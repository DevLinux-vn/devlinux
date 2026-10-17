/* =============================================================================
 * Session 04 - Single-digit 7-segment counter, button driven by a GPIO INTERRUPT.
 *
 * DISPLAY TYPE: COMMON CATHODE.
 *   Common pin to GND, so a segment lights when its GPIO is HIGH. For a common
 *   anode part flip SEG_ACTIVE_HIGH to 0 - the digit table is never duplicated.
 *
 * TARGET: ESP32-S3.
 *
 * Two layers deliberately coexist here:
 *   - the DISPLAY is still driven at register level (IO_MUX + GPIO matrix +
 *     GPIO_OUT_* / GPIO_ENABLE_*), copied from Session 03;
 *   - the BUTTON now goes through the Driver API with an ANYEDGE interrupt.
 *
 * No loop anywhere reads the button pin. Remove the interrupt and the program
 * goes completely deaf.
 * ========================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* --- register level, for the display only --- */
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"

/* --- driver level, for the button only --- */
#include "driver/gpio.h"
#include "esp_attr.h"        /* IRAM_ATTR */
#include "esp_timer.h"       /* esp_timer_get_time() - safe from an ISR */
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/* =============================================================================
 * 1. Pins
 * ========================================================================== */

#define SEG_A_PIN (4U)
#define SEG_B_PIN (5U)
#define SEG_C_PIN (6U)
#define SEG_D_PIN (7U)
#define SEG_E_PIN (15U)
#define SEG_F_PIN (17U)
#define SEG_G_PIN (16U)

/* Matches the wiring actually on the board. The brief says GPIO14; if the
   button is moved there, change this line only. */
#define BTN_PIN   GPIO_NUM_14

#define SEG_COUNT (7U)

/* Bit b0..b6 = segments a..g. This table is for a COMMON CATHODE display. */
static const uint8_t SEGMENT_MAP[10] = {
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U, /* 0 1 2 3 4 */
    0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU, /* 5 6 7 8 9 */
};

#define DIGIT_COUNT (10U)

/* The single inversion switch. 1 = common cathode, 0 = common anode. */
#define SEG_ACTIVE_HIGH (1)

/* Button to GND with the pad's internal pull-up: pressed reads LOW. */
#define BTN_LEVEL_PRESSED (0)

/* =============================================================================
 * 2. Timing
 * ========================================================================== */

#define DEBOUNCE_MS       (25U)
#define DOUBLE_CLICK_MS   (350U)  /* max gap between two clicks to count as a double */
#define LONG_PRESS_MS     (800U)  /* hold time before auto-repeat starts        */
#define REPEAT_PERIOD_MS  (500U)  /* required by the spec — do not change this one */

#define US_PER_MS (1000LL)

#define BTN_QUEUE_LEN     (16U)   /* bounce can burst; leave room              */
#define GESTURE_TASK_STACK (3072U)
#define GESTURE_TASK_PRIO  (5U)

/* =============================================================================
 * 3. Register layout for the display (TRM: "IO MUX and GPIO Matrix")
 * ========================================================================== */

#define IO_MUX_MCU_SEL_SHIFT   (12U)
#define IO_MUX_MCU_SEL_MASK    (0x7U)
#define IO_MUX_FUNC_GPIO       (1U)

#define IO_MUX_FUN_DRV_SHIFT   (10U)
#define IO_MUX_FUN_DRV_MASK    (0x3U)
#define IO_MUX_DRV_DEFAULT     (2U)

#define IO_MUX_FUN_IE_BIT      (1U << 9U)
#define IO_MUX_FUN_WPU_BIT     (1U << 8U)
#define IO_MUX_FUN_WPD_BIT     (1U << 7U)

#define GPIO_MATRIX_OUT_SEL_BASE  (GPIO_FUNC0_OUT_SEL_CFG_REG)
#define GPIO_MATRIX_OUT_SEL_STEP  (4U)
/* On the ESP32-S3, index 256 means "take the level straight from GPIO_OUT_REG".
   Chip-specific: 128 lands on a real I2S signal and the pad stays dark. */
#define GPIO_MATRIX_SIG_GPIO_OUT  (SIG_GPIO_OUT_IDX)

/* =============================================================================
 * 4. Volatile register accessors
 * ========================================================================== */

static inline uint32_t reg_read(uint32_t addr)
{
    return *(volatile uint32_t *)addr;
}

static inline void reg_write(uint32_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
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
 * 5. Display - unchanged from Session 03
 * ========================================================================== */

static const char *TAG = "BTN";

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
        default:        return 0U;
    }
}

/* Order must match SEGMENT_MAP bits b0..b6 = a..g */
static const uint32_t seg_pins[SEG_COUNT] = {
    SEG_A_PIN, SEG_B_PIN, SEG_C_PIN,
    SEG_D_PIN, SEG_E_PIN, SEG_F_PIN, SEG_G_PIN,
};

static uint32_t seg_all_mask;
static uint32_t digit_high_mask[DIGIT_COUNT];

static void configure_output_pin(uint32_t pin)
{
    uint32_t mux = io_mux_reg_for_pin(pin);

    reg_set_field(mux, IO_MUX_MCU_SEL_SHIFT, IO_MUX_MCU_SEL_MASK, IO_MUX_FUNC_GPIO);
    reg_set_field(mux, IO_MUX_FUN_DRV_SHIFT, IO_MUX_FUN_DRV_MASK, IO_MUX_DRV_DEFAULT);
    reg_clear_bits(mux, IO_MUX_FUN_IE_BIT | IO_MUX_FUN_WPU_BIT | IO_MUX_FUN_WPD_BIT);

    reg_write(GPIO_MATRIX_OUT_SEL_BASE + (pin * GPIO_MATRIX_OUT_SEL_STEP),
              GPIO_MATRIX_SIG_GPIO_OUT);
}

/* The ONLY place the common-cathode / common-anode difference lives. */
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

#if SEG_ACTIVE_HIGH
    reg_write(GPIO_OUT_W1TC_REG, seg_all_mask);
#else
    reg_write(GPIO_OUT_W1TS_REG, seg_all_mask);
#endif
    reg_write(GPIO_ENABLE_W1TS_REG, seg_all_mask);
}

/* =============================================================================
 * 6. Button: ISR -> queue
 * ========================================================================== */

typedef struct {
    int64_t timestamp_us;
    bool    is_press;   /* falling edge = press, rising edge = release */
} btn_event_t;

static QueueHandle_t btn_queue;

/* Capture what happened and when, hand it off, return. Nothing else.
   esp_timer_get_time() and gpio_get_level() are both IRAM-resident, so this
   handler is safe even while the flash cache is disabled. */
static void IRAM_ATTR button_isr(void *arg)
{
    (void)arg;

    btn_event_t ev = {
        .timestamp_us = esp_timer_get_time(),
        .is_press     = (gpio_get_level(BTN_PIN) == BTN_LEVEL_PRESSED),
    };

    BaseType_t hp_task_woken = pdFALSE;
    xQueueSendFromISR(btn_queue, &ev, &hp_task_woken);

    /* xQueueSendFromISR never blocks; instead it reports whether it unblocked a
       task more important than the one that was interrupted. If so we ask for a
       context switch on the way out, otherwise that task waits until the next
       scheduler tick. */
    if (hp_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void button_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BTN_PIN),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_ANYEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BTN_PIN, button_isr, NULL));
}

/* =============================================================================
 * 7. Gesture decoding task
 * ========================================================================== */

static uint8_t digit_value;

static void digit_step(int delta)
{
    int v = (int)digit_value + delta;
    if (v < 0) { v += (int)DIGIT_COUNT; }
    if (v >= (int)DIGIT_COUNT) { v -= (int)DIGIT_COUNT; }
    digit_value = (uint8_t)v;
    seg7_show_digit(digit_value);
}

/* How long may we sleep before something is due? The state decides.
   Idle with nothing pending -> sleep forever, zero CPU. */
static TickType_t next_deadline_ticks(bool btn_down, bool repeating,
                                      bool click_pending,
                                      int64_t press_us, int64_t next_repeat_us,
                                      int64_t click_us, int64_t now_us)
{
    int64_t due_us = INT64_MAX;

    if (btn_down && !repeating) {
        due_us = press_us + ((int64_t)LONG_PRESS_MS * US_PER_MS);
    } else if (btn_down && repeating) {
        due_us = next_repeat_us;
    } else if (click_pending) {
        due_us = click_us + ((int64_t)DOUBLE_CLICK_MS * US_PER_MS);
    }

    if (due_us == INT64_MAX) {
        return portMAX_DELAY;
    }

    int64_t remain_us = due_us - now_us;
    if (remain_us <= 0) {
        return 0;
    }
    /* Round up so we never wake a hair early and spin. */
    return pdMS_TO_TICKS((uint32_t)((remain_us + US_PER_MS - 1) / US_PER_MS)) + 1;
}

static void gesture_task(void *arg)
{
    (void)arg;

    bool    btn_down       = false;
    int64_t press_us       = 0;
    int64_t last_edge_us   = INT64_MIN / 2;   /* last edge we accepted */

    bool    repeating      = false;
    int64_t next_repeat_us = 0;

    bool    click_pending  = false;
    int64_t click_us       = 0;

    while (1) {
        int64_t now_us = esp_timer_get_time();
        TickType_t wait = next_deadline_ticks(btn_down, repeating, click_pending,
                                              press_us, next_repeat_us,
                                              click_us, now_us);

        btn_event_t ev;
        if (xQueueReceive(btn_queue, &ev, wait) == pdTRUE) {
            /* ---- debounce, in software, on timestamps ---------------------
               Every bounce edge produced a real interrupt and a real queue
               entry. An edge arriving within DEBOUNCE_MS of the last accepted
               edge is contact chatter, not a human. Also drop any edge that
               claims a state we are already in. */
            bool too_soon = (ev.timestamp_us - last_edge_us)
                            < ((int64_t)DEBOUNCE_MS * US_PER_MS);
            if (too_soon || ev.is_press == btn_down) {
                continue;
            }

            last_edge_us = ev.timestamp_us;
            btn_down     = ev.is_press;

            if (ev.is_press) {
                press_us  = ev.timestamp_us;
                repeating = false;
            } else {
                if (repeating) {
                    /* End of a hold. Deliberately NOT a click. */
                    repeating = false;
                    ESP_LOGI(TAG, "LONG end");
                } else if (click_pending &&
                           (ev.timestamp_us - click_us)
                               <= ((int64_t)DOUBLE_CLICK_MS * US_PER_MS)) {
                    click_pending = false;
                    digit_step(-1);
                    ESP_LOGI(TAG, "DOUBLE     -> %u", digit_value);
                } else {
                    /* First click. Cannot act yet - a partner may follow. */
                    click_pending = true;
                    click_us      = ev.timestamp_us;
                }
            }
        }

        /* ---- deadlines, checked whether we woke on an edge or a timeout ---
           While the button is held down no further edges arrive, so the repeat
           has to come from the timeout half of xQueueReceive(). */
        now_us = esp_timer_get_time();

        if (btn_down && !repeating &&
            (now_us - press_us) >= ((int64_t)LONG_PRESS_MS * US_PER_MS)) {
            /* A hold overtakes any click still waiting: commit it first so the
               earlier click is not silently swallowed. */
            if (click_pending) {
                click_pending = false;
                digit_step(+1);
                ESP_LOGI(TAG, "CLICK      -> %u", digit_value);
            }
            repeating      = true;
            next_repeat_us = now_us + ((int64_t)REPEAT_PERIOD_MS * US_PER_MS);
            digit_step(+1);
            ESP_LOGI(TAG, "LONG start -> %u", digit_value);
        } else if (btn_down && repeating && now_us >= next_repeat_us) {
            /* Advance by a fixed step, not from "now", so the repeat rate does
               not drift with scheduling jitter. */
            next_repeat_us += ((int64_t)REPEAT_PERIOD_MS * US_PER_MS);
            digit_step(+1);
            ESP_LOGI(TAG, "LONG rep   -> %u", digit_value);
        }

        if (click_pending &&
            (now_us - click_us) > ((int64_t)DOUBLE_CLICK_MS * US_PER_MS)) {
            click_pending = false;
            digit_step(+1);
            ESP_LOGI(TAG, "CLICK      -> %u", digit_value);
        }
    }
}

/* =============================================================================
 * 8. app_main
 * ========================================================================== */

void app_main(void)
{
    seg7_init();
    digit_value = 0U;
    seg7_show_digit(digit_value);

    /* Queue must exist before the ISR can fire into it. */
    btn_queue = xQueueCreate(BTN_QUEUE_LEN, sizeof(btn_event_t));
    configASSERT(btn_queue != NULL);

    xTaskCreate(gesture_task, "gesture", GESTURE_TASK_STACK, NULL,
                GESTURE_TASK_PRIO, NULL);

    /* Installed last: by now both the queue and its consumer are ready. */
    button_init();
}
