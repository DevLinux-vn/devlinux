/* =============================================================================
 * Seven-segment digit counter with a single push button.
 *
 * DISPLAY TYPE: COMMON CATHODE.
 *   The common pin is tied to GND, so a segment lights when its GPIO is HIGH.
 *   For a common anode part, flip SEG_ACTIVE_HIGH to 0 and nothing else.
 *   SEGMENT_MAP is never duplicated - the inversion happens in exactly one
 *   place, inside build_digit_masks().
 *
 * TARGET: ESP32-S3.
 * Register access only - no driver/gpio.h, no gpio_config/set_level/get_level.
 * Button handling is polled, no GPIO interrupts.
 * ========================================================================== */

#include <stdint.h>
#include <stdbool.h>

#include "soc/io_mux_reg.h"   /* IO_MUX_GPIOn_REG addresses (from the TRM)     */
#include "soc/gpio_reg.h"     /* GPIO_OUT_*, GPIO_ENABLE_*, GPIO_IN_REG        */
#include "soc/gpio_sig_map.h" /* SIG_GPIO_OUT_IDX                              */

#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* =============================================================================
 * 1. Named constants - pins
 * ========================================================================== */

#define SEG_A_PIN (4U)
#define SEG_B_PIN (5U)
#define SEG_C_PIN (6U)
#define SEG_D_PIN (7U)
#define SEG_E_PIN (15U)
#define SEG_F_PIN (17U)
#define SEG_G_PIN (16U)
#define BTN_PIN   (14U)

#define SEG_COUNT (7U)

/* Bit b0..b6 = segments a..g. This table is for a COMMON CATHODE display. */
static const uint8_t SEGMENT_MAP[10] = {
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U, /* 0 1 2 3 4 */
    0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU, /* 5 6 7 8 9 */
};

#define DIGIT_COUNT (10U)

/* The single inversion switch. 1 = common cathode, 0 = common anode. */
#define SEG_ACTIVE_HIGH (1)

/* Button wired to GND with the pad's internal pull-up: pressed reads LOW. */
#define BTN_LEVEL_PRESSED  (0U)
#define BTN_LEVEL_RELEASED (1U)

/* =============================================================================
 * 2. Named constants - timing
 * ========================================================================== */

#define POLL_PERIOD_MS    (10U)   /* fixed polling period                       */
#define DEBOUNCE_MS       (25U)
#define DOUBLE_CLICK_MS   (350U)  /* max gap between two clicks to count as a double */
#define LONG_PRESS_MS     (800U)  /* hold time before auto-repeat starts        */
#define REPEAT_PERIOD_MS  (500U)  /* required by the spec — do not change this one */

#define US_PER_MS (1000LL)

/* =============================================================================
 * 3. Named constants - register layout (TRM ch. "IO MUX and GPIO Matrix")
 * ========================================================================== */

/* --- IO_MUX_GPIOn_REG field positions --- */
#define IO_MUX_MCU_SEL_SHIFT   (12U)  /* function select, 3 bits               */
#define IO_MUX_MCU_SEL_MASK    (0x7U)
#define IO_MUX_FUNC_GPIO       (1U)   /* MCU_SEL value routing pad to GPIO matrix */

#define IO_MUX_FUN_DRV_SHIFT   (10U)  /* drive strength, 2 bits                */
#define IO_MUX_FUN_DRV_MASK    (0x3U)
#define IO_MUX_DRV_DEFAULT     (2U)   /* ~20 mA, plenty through a 220R resistor */

#define IO_MUX_FUN_IE_BIT      (1U << 9U)   /* input buffer enable             */
#define IO_MUX_FUN_WPU_BIT     (1U << 8U)   /* internal pull-up                */
#define IO_MUX_FUN_WPD_BIT     (1U << 7U)   /* internal pull-down              */

/* --- GPIO matrix output routing --- */
/* GPIO_FUNCn_OUT_SEL_CFG_REG = base + 4*n. Writing SIG_GPIO_OUT means
   "this pad takes its level straight from GPIO_OUT_REG"; leaving OEN_SEL at 0
   means "the output driver is controlled by GPIO_ENABLE_REG". */
#define GPIO_MATRIX_OUT_SEL_BASE  (GPIO_FUNC0_OUT_SEL_CFG_REG)
#define GPIO_MATRIX_OUT_SEL_STEP  (4U)
/* On the ESP32-S3 the peripheral signals are numbered 0..255, and index 256
   is the special "take the level straight from GPIO_OUT_REG" value.
   Note this is chip-specific - writing 128 here lands on a real I2S signal and
   the pad stays dark. Taken from the header rather than hard-coded. */
#define GPIO_MATRIX_SIG_GPIO_OUT  (SIG_GPIO_OUT_IDX)

/* All pins used here are below 32, so only the low register bank is involved. */
#define GPIO_BANK_LOW_MAX_PIN     (31U)

/* =============================================================================
 * 4. Volatile register accessors - every register touch goes through these
 * ========================================================================== */

static inline uint32_t reg_read(uint32_t addr)
{
    return *(volatile uint32_t *)addr;
}

static inline void reg_write(uint32_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}

static inline void reg_set_bits(uint32_t addr, uint32_t bits)
{
    reg_write(addr, reg_read(addr) | bits);
}

static inline void reg_clear_bits(uint32_t addr, uint32_t bits)
{
    reg_write(addr, reg_read(addr) & ~bits);
}

static inline void reg_set_field(uint32_t addr, uint32_t shift,
                                 uint32_t mask, uint32_t value)
{
    uint32_t v = reg_read(addr);
    v &= ~(mask << shift);
    v |= ((value & mask) << shift);
    reg_write(addr, v);
}

/* =============================================================================
 * 5. Pad configuration
 * ========================================================================== */

static const char *TAG = "seg7";

/* IO_MUX register address per pin. Explicit table instead of arithmetic so a
   wrong pin number is a compile error rather than a silent wrong address. */
static uint32_t io_mux_reg_for_pin(uint32_t pin)
{
    switch (pin) {
        case SEG_A_PIN: return IO_MUX_GPIO4_REG;
        case SEG_B_PIN: return IO_MUX_GPIO5_REG;
        case SEG_C_PIN: return IO_MUX_GPIO6_REG;
        case SEG_D_PIN: return IO_MUX_GPIO7_REG;
        case SEG_E_PIN: return IO_MUX_GPIO15_REG;
        case SEG_G_PIN: return IO_MUX_GPIO16_REG;
        case SEG_F_PIN: return IO_MUX_GPIO17_REG;
        case BTN_PIN:   return IO_MUX_GPIO14_REG;
        default:        return 0U;
    }
}

/* Order must match SEGMENT_MAP bits b0..b6 = a..g */
static const uint32_t seg_pins[SEG_COUNT] = {
    SEG_A_PIN, SEG_B_PIN, SEG_C_PIN,
    SEG_D_PIN, SEG_E_PIN, SEG_F_PIN, SEG_G_PIN,
};

static uint32_t seg_all_mask;              /* every segment bit                */
static uint32_t digit_high_mask[DIGIT_COUNT]; /* pre-built: bits to drive HIGH */

/* Step 1 (pad) + step 2 (matrix) + step 3 (driver) for one output pin. */
static void configure_output_pin(uint32_t pin)
{
    uint32_t mux = io_mux_reg_for_pin(pin);

    /* Pad -> plain GPIO function. Without this the pad stays on whatever
       peripheral function it defaults to and the segment never lights. */
    reg_set_field(mux, IO_MUX_MCU_SEL_SHIFT, IO_MUX_MCU_SEL_MASK, IO_MUX_FUNC_GPIO);
    reg_set_field(mux, IO_MUX_FUN_DRV_SHIFT, IO_MUX_FUN_DRV_MASK, IO_MUX_DRV_DEFAULT);

    /* Output only: input buffer off, both pulls off (external resistor present). */
    reg_clear_bits(mux, IO_MUX_FUN_IE_BIT | IO_MUX_FUN_WPU_BIT | IO_MUX_FUN_WPD_BIT);

    /* GPIO matrix: pad takes its data from GPIO_OUT_REG. */
    reg_write(GPIO_MATRIX_OUT_SEL_BASE + (pin * GPIO_MATRIX_OUT_SEL_STEP),
              GPIO_MATRIX_SIG_GPIO_OUT);
}

static void configure_button_pin(void)
{
    uint32_t mux = io_mux_reg_for_pin(BTN_PIN);

    reg_set_field(mux, IO_MUX_MCU_SEL_SHIFT, IO_MUX_MCU_SEL_MASK, IO_MUX_FUNC_GPIO);

    /* Input buffer ON and internal pull-up ON - the switch pulls the pin to
       GND when pressed, so no external resistor is needed. */
    reg_set_bits(mux, IO_MUX_FUN_IE_BIT | IO_MUX_FUN_WPU_BIT);
    reg_clear_bits(mux, IO_MUX_FUN_WPD_BIT);

    /* Leave the output driver off so the pin stays an input. */
    reg_write(GPIO_ENABLE_W1TC_REG, (1UL << BTN_PIN));
}

/* =============================================================================
 * 6. Display
 * ========================================================================== */

/* Pre-compute, for each digit, the set of pins that must be driven HIGH.
   This is the ONLY place the common-cathode / common-anode difference lives. */
static void build_digit_masks(void)
{
    seg_all_mask = 0U;
    for (uint32_t i = 0; i < SEG_COUNT; i++) {
        seg_all_mask |= (1UL << seg_pins[i]);
    }

    for (uint32_t d = 0; d < DIGIT_COUNT; d++) {
        uint32_t lit = 0U;
        for (uint32_t i = 0; i < SEG_COUNT; i++) {
            if ((SEGMENT_MAP[d] >> i) & 0x01U) {
                lit |= (1UL << seg_pins[i]);
            }
        }
#if SEG_ACTIVE_HIGH
        digit_high_mask[d] = lit;
#else
        digit_high_mask[d] = seg_all_mask & ~lit;
#endif
    }
}

/* Two writes, no read. W1TS/W1TC only touch the bits set in the written value,
   so segments update as one atomic pair with zero risk of clobbering an
   unrelated pin - which a read-modify-write of GPIO_OUT_REG cannot promise if
   an interrupt lands between the read and the write. */
static void seg7_show_digit(uint8_t digit)
{
    if (digit >= DIGIT_COUNT) {
        return;
    }
    uint32_t high = digit_high_mask[digit];
    reg_write(GPIO_OUT_W1TC_REG, seg_all_mask & ~high);
    reg_write(GPIO_OUT_W1TS_REG, high);
}

static void seg7_init(void)
{
    build_digit_masks();

    for (uint32_t i = 0; i < SEG_COUNT; i++) {
        configure_output_pin(seg_pins[i]);
    }

    /* Blank first, then enable the drivers, so nothing flickers at boot. */
#if SEG_ACTIVE_HIGH
    reg_write(GPIO_OUT_W1TC_REG, seg_all_mask);
#else
    reg_write(GPIO_OUT_W1TS_REG, seg_all_mask);
#endif
    reg_write(GPIO_ENABLE_W1TS_REG, seg_all_mask);
}

/* =============================================================================
 * 7. Button: raw read + debounce
 * ========================================================================== */

static inline uint32_t btn_read_raw(void)
{
    return (reg_read(GPIO_IN_REG) >> BTN_PIN) & 0x1U;
}

/* A level must hold steady for DEBOUNCE_MS before it is believed. */
typedef struct {
    uint32_t stable;      /* the level we currently trust                     */
    uint32_t candidate;   /* the level we are timing                          */
    int64_t  since_us;    /* when candidate first appeared                    */
} debounce_t;

static void debounce_init(debounce_t *d, int64_t now_us)
{
    d->stable    = BTN_LEVEL_RELEASED;
    d->candidate = BTN_LEVEL_RELEASED;
    d->since_us  = now_us;
}

static uint32_t debounce_update(debounce_t *d, uint32_t raw, int64_t now_us)
{
    if (raw != d->candidate) {
        d->candidate = raw;
        d->since_us  = now_us;
    } else if (raw != d->stable &&
               (now_us - d->since_us) >= ((int64_t)DEBOUNCE_MS * US_PER_MS)) {
        d->stable = raw;
    }
    return d->stable;
}

/* =============================================================================
 * 8. Gesture decoding
 * ========================================================================== */

static uint8_t digit_value;

static void digit_step(int delta)
{
    int v = (int)digit_value + delta;
    if (v < 0)  { v += (int)DIGIT_COUNT; }
    if (v >= (int)DIGIT_COUNT) { v -= (int)DIGIT_COUNT; }
    digit_value = (uint8_t)v;
    seg7_show_digit(digit_value);
}

void app_main(void)
{
    seg7_init();
    configure_button_pin();

    digit_value = 0U;
    seg7_show_digit(digit_value);

    int64_t now_us = esp_timer_get_time();

    debounce_t btn;
    debounce_init(&btn, now_us);

    bool    was_pressed   = false;
    int64_t press_us      = 0;

    bool    repeating     = false;  /* auto-repeat active for this hold        */
    int64_t last_repeat_us = 0;

    bool    click_pending = false;  /* one click seen, waiting out the double  */
    int64_t click_us      = 0;      /* window; may still become a decrement    */

    while (1) {
        now_us = esp_timer_get_time();

        uint32_t level = debounce_update(&btn, btn_read_raw(), now_us);
        bool pressed = (level == BTN_LEVEL_PRESSED);

        /* ---- edge: press ------------------------------------------------ */
        if (pressed && !was_pressed) {
            press_us  = now_us;
            repeating = false;
        }

        /* ---- edge: release ---------------------------------------------- */
        if (!pressed && was_pressed) {
            if (repeating) {
                /* End of a long press. Deliberately NOT a click - the hold
                   already did its counting. */
                repeating = false;
                ESP_LOGI(TAG, "hold ended");
            } else if (click_pending &&
                       (now_us - click_us) <= ((int64_t)DOUBLE_CLICK_MS * US_PER_MS)) {
                /* Second click inside the window: the pending click is
                   cancelled, the pair means decrement. */
                click_pending = false;
                digit_step(-1);
                ESP_LOGI(TAG, "double click -> %u", digit_value);
            } else {
                /* First click. Cannot act yet - a partner may be coming. */
                click_pending = true;
                click_us      = now_us;
            }
        }

        /* ---- held: long press starts, then auto-repeats ------------------ */
        if (pressed) {
            if (!repeating &&
                (now_us - press_us) >= ((int64_t)LONG_PRESS_MS * US_PER_MS)) {
                /* A hold overtakes any click still waiting: commit it first so
                   the earlier click is not silently lost. */
                if (click_pending) {
                    click_pending = false;
                    digit_step(+1);
                }
                repeating      = true;
                last_repeat_us = now_us;
                digit_step(+1);
                ESP_LOGI(TAG, "hold -> %u", digit_value);
            } else if (repeating &&
                       (now_us - last_repeat_us) >= ((int64_t)REPEAT_PERIOD_MS * US_PER_MS)) {
                last_repeat_us += ((int64_t)REPEAT_PERIOD_MS * US_PER_MS);
                digit_step(+1);
                ESP_LOGI(TAG, "repeat -> %u", digit_value);
            }
        }

        /* ---- pending click times out: commit it as a single click -------- */
        if (click_pending &&
            (now_us - click_us) > ((int64_t)DOUBLE_CLICK_MS * US_PER_MS)) {
            click_pending = false;
            digit_step(+1);
            ESP_LOGI(TAG, "single click -> %u", digit_value);
        }

        was_pressed = pressed;
        vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS));
    }
}
