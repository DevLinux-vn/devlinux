/*
 * ESP32-S3: 1-digit 7-segment counter with one button.
 * Single click: +1, double click: -1, long press: +1 then +1 every 500 ms.
 * Bare-metal GPIO register access; FreeRTOS only for periodic polling.
 *
 * Wiring (active-low pushbutton): GPIO14 ----- button ----- GND
 * LED a,b,c,d,e,f,g,dp: GPIO4,5,6,7,15,16,17,18, respectively.
 * Set DISPLAY_COMMON_ANODE to match the display hardware.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* --------------------- Hardware and register constants ------------------ */
#define DISPLAY_COMMON_ANODE          0U   /* 0: cathode to GND; 1: anode to 3V3 */ // Pick cathode mode

#define DR_REG_GPIO_BASE              0x60004000UL
#define DR_REG_IO_MUX_BASE            0x60009000UL

#define GPIO_OUT_W1TS_OFFSET          0x0008UL
#define GPIO_OUT_W1TC_OFFSET          0x000CUL
#define GPIO_ENABLE_W1TS_OFFSET       0x0024UL
#define GPIO_ENABLE_W1TC_OFFSET       0x0028UL
#define GPIO_IN_OFFSET                0x003CUL
#define GPIO_FUNC_OUT_SEL0_OFFSET     0x0554UL
#define IO_MUX_GPIO0_OFFSET           0x0004UL
#define REG_STRIDE                    4UL

#define REG32(address)                (*(volatile uint32_t *)(uintptr_t)(address))
#define REG_GPIO_OUT_W1TS            REG32(DR_REG_GPIO_BASE + GPIO_OUT_W1TS_OFFSET)
#define REG_GPIO_OUT_W1TC            REG32(DR_REG_GPIO_BASE + GPIO_OUT_W1TC_OFFSET)
#define REG_GPIO_ENABLE_W1TS         REG32(DR_REG_GPIO_BASE + GPIO_ENABLE_W1TS_OFFSET)
#define REG_GPIO_ENABLE_W1TC         REG32(DR_REG_GPIO_BASE + GPIO_ENABLE_W1TC_OFFSET)
#define REG_GPIO_IN                  REG32(DR_REG_GPIO_BASE + GPIO_IN_OFFSET)
#define REG_GPIO_FUNC_OUT_SEL(pin)    REG32(DR_REG_GPIO_BASE + GPIO_FUNC_OUT_SEL0_OFFSET + (uint32_t)(pin) * REG_STRIDE)

/* The linear IO_MUX address mapping is valid here for GPIO0...GPIO21. */
#define REG_IO_MUX_GPIO(pin)          REG32(DR_REG_IO_MUX_BASE + IO_MUX_GPIO0_OFFSET + (uint32_t)(pin) * REG_STRIDE)

#define IO_MUX_MCU_SEL_GPIO           (1UL << 12)
#define IO_MUX_FUN_IE                 (1UL << 9)
#define IO_MUX_FUN_WPU                (1UL << 8)
#define IO_MUX_FUN_DRV_SHIFT          10U
#define IO_MUX_FUN_DRV_DEFAULT        (2UL << IO_MUX_FUN_DRV_SHIFT)
#define GPIO_MATRIX_SIMPLE_OUT_IDX    0x100UL
#define GPIO_MATRIX_OEN_SEL_BIT       10U
#define GPIO_MATRIX_OEN_FROM_GPIO     (1UL << GPIO_MATRIX_OEN_SEL_BIT)

#define PIN_SEG_A                     4U
#define PIN_SEG_B                     5U
#define PIN_SEG_C                     6U
#define PIN_SEG_D                     7U
#define PIN_SEG_E                     15U
#define PIN_SEG_F                     16U
#define PIN_SEG_G                     17U
#define PIN_SEG_DP                    18U
#define PIN_BUTTON                    14U
#define GPIO_BIT(pin)                 (1UL << (pin))

#define ALL_SEGS_MASK                (GPIO_BIT(PIN_SEG_A) | GPIO_BIT(PIN_SEG_B) | \
                                      GPIO_BIT(PIN_SEG_C) | GPIO_BIT(PIN_SEG_D) | \
                                      GPIO_BIT(PIN_SEG_E) | GPIO_BIT(PIN_SEG_F) | \
                                      GPIO_BIT(PIN_SEG_G) | GPIO_BIT(PIN_SEG_DP))

/* ------------------------ Timings and digit table ------------------------ */
#define DEBOUNCE_TIME_MS              25U
#define DOUBLE_CLICK_WINDOW_MS        350U
#define LONG_PRESS_TIME_MS            800U
#define LONG_PRESS_REPEAT_MS          500U
#define BUTTON_POLL_INTERVAL_MS       5U
#define DIGIT_COUNT                   10U

/* bit0...bit6 = segment a...g; decimal point stays OFF. */
static const uint8_t s_digit_patterns[DIGIT_COUNT] = {
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U,
    0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU
};
static const uint8_t s_segment_pins[7] = {
    PIN_SEG_A, PIN_SEG_B, PIN_SEG_C, PIN_SEG_D,
    PIN_SEG_E, PIN_SEG_F, PIN_SEG_G
};

static uint32_t compute_segment_mask(uint8_t pattern)
{
    uint32_t mask = 0U;
    for (uint32_t i = 0; i < 7U; ++i) {
        if ((pattern & (1U << i)) != 0U) {
            mask |= GPIO_BIT(s_segment_pins[i]);
        }
    }
    return mask;
}

static void display_digit(uint8_t digit)
{
    if (digit >= DIGIT_COUNT) {
        return;
    }

    const uint32_t active_mask = compute_segment_mask(s_digit_patterns[digit]);
#if DISPLAY_COMMON_ANODE
    /* First blank the display; active LED segment = LOW. */
    REG_GPIO_OUT_W1TS = ALL_SEGS_MASK;
    REG_GPIO_OUT_W1TC = active_mask;
#else
    /* First blank the display; active LED segment = HIGH. */
    REG_GPIO_OUT_W1TC = ALL_SEGS_MASK;
    REG_GPIO_OUT_W1TS = active_mask;
#endif
}

static void hardware_init(void)
{
    static const uint8_t output_pins[] = {
        PIN_SEG_A, PIN_SEG_B, PIN_SEG_C, PIN_SEG_D,
        PIN_SEG_E, PIN_SEG_F, PIN_SEG_G, PIN_SEG_DP
    };

    /* Pre-load OFF levels before enabling physical outputs. */
#if DISPLAY_COMMON_ANODE
    REG_GPIO_OUT_W1TS = ALL_SEGS_MASK;
#else
    REG_GPIO_OUT_W1TC = ALL_SEGS_MASK;
#endif

    for (size_t i = 0; i < sizeof(output_pins) / sizeof(output_pins[0]); ++i) {
        const uint8_t pin = output_pins[i];
        REG_IO_MUX_GPIO(pin) = IO_MUX_MCU_SEL_GPIO | IO_MUX_FUN_DRV_DEFAULT;
        REG_GPIO_FUNC_OUT_SEL(pin) = GPIO_MATRIX_SIMPLE_OUT_IDX | GPIO_MATRIX_OEN_FROM_GPIO;
        REG_GPIO_ENABLE_W1TS = GPIO_BIT(pin);
    }

    /* Input buffer + internal pull-up; pressed means GPIO14 reads LOW. */
    REG_GPIO_ENABLE_W1TC = GPIO_BIT(PIN_BUTTON);
    REG_IO_MUX_GPIO(PIN_BUTTON) = IO_MUX_MCU_SEL_GPIO | IO_MUX_FUN_IE | IO_MUX_FUN_WPU;
}

static bool read_button_pressed(void)
{
    return (REG_GPIO_IN & GPIO_BIT(PIN_BUTTON)) == 0U;
}

static uint32_t get_time_ms(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000LL);
}

/* ------------------------------- Button FSM ----------------------------- */
typedef enum {
    STATE_IDLE = 0,
    STATE_WAIT_RELEASE_1,
    STATE_WAIT_DOUBLE,
    STATE_SECOND_PRESS,
    STATE_WAIT_RELEASE_2,
    STATE_LONG_PRESS
} button_state_t;

typedef enum {
    BTN_EVENT_NONE = 0,
    BTN_EVENT_SINGLE_CLICK,
    BTN_EVENT_DOUBLE_CLICK,
    BTN_EVENT_LONG_PRESS_TICK
} button_event_t;

typedef struct {
    button_state_t state;
    bool raw_prev;
    bool stable_pressed;
    uint32_t raw_changed_at;
    uint32_t press_anchor;
    uint32_t release_anchor;
    uint32_t second_press_anchor;
    uint32_t repeat_anchor;
    button_event_t queued_event;
} button_fsm_t;

/* Starts in STATE_IDLE, with button released. */
static button_fsm_t s_button = {0};

static button_event_t poll_button_event(bool raw_pressed, uint32_t now)
{
    bool press_edge = false;
    bool release_edge = false;
    button_event_t evt = BTN_EVENT_NONE;

    /* Debounce: the raw level must remain unchanged for 25 ms. */
    if (raw_pressed != s_button.raw_prev) {
        s_button.raw_prev = raw_pressed;
        s_button.raw_changed_at = now;
    }

    if (raw_pressed != s_button.stable_pressed &&
        (uint32_t)(now - s_button.raw_changed_at) >= DEBOUNCE_TIME_MS) {
        s_button.stable_pressed = raw_pressed;
        press_edge = raw_pressed;
        release_edge = !raw_pressed;
    }

    switch (s_button.state) {
    case STATE_IDLE:
        if (press_edge) {
            s_button.press_anchor = s_button.raw_changed_at;
            s_button.state = STATE_WAIT_RELEASE_1;
        }
        break;

    case STATE_WAIT_RELEASE_1:
        if (release_edge) {
            s_button.release_anchor = s_button.raw_changed_at;
            s_button.state = STATE_WAIT_DOUBLE;
        } else if (s_button.stable_pressed && raw_pressed &&
                   (uint32_t)(now - s_button.press_anchor) >= LONG_PRESS_TIME_MS) {
            s_button.state = STATE_LONG_PRESS;
            s_button.repeat_anchor = now;
            evt = BTN_EVENT_LONG_PRESS_TICK;
        }
        break;

    case STATE_WAIT_DOUBLE:
        /* Candidate second press must BEGIN inside the 350-ms window. */
        if (raw_pressed &&
            (uint32_t)(s_button.raw_changed_at - s_button.release_anchor) < DOUBLE_CLICK_WINDOW_MS) {
            s_button.state = STATE_SECOND_PRESS;
        } else if ((uint32_t)(now - s_button.release_anchor) >= DOUBLE_CLICK_WINDOW_MS) {
            evt = BTN_EVENT_SINGLE_CLICK;
            s_button.state = STATE_IDLE;
            /* A new, unrelated press may already have passed debounce. */
            if (press_edge) {
                s_button.press_anchor = s_button.raw_changed_at;
                s_button.state = STATE_WAIT_RELEASE_1;
            }
        }
        break;

    case STATE_SECOND_PRESS:
        if (press_edge) {
            s_button.second_press_anchor = s_button.raw_changed_at;
            s_button.state = STATE_WAIT_RELEASE_2;
        } else if (!raw_pressed) {
            /* A brief glitch is not a double click: preserve the first click. */
            s_button.state = STATE_WAIT_DOUBLE;
        }
        break;

    case STATE_WAIT_RELEASE_2:
        if (release_edge) {
            evt = BTN_EVENT_DOUBLE_CLICK;
            s_button.state = STATE_IDLE;
        } else if (s_button.stable_pressed && raw_pressed &&
                   (uint32_t)(now - s_button.second_press_anchor) >= LONG_PRESS_TIME_MS) {
            /* First tap + a second LONG hold are two separate actions:
             * send one long-press increment now, and the first click next poll.
             * It must NOT be treated as a double click.
             */
            s_button.state = STATE_LONG_PRESS;
            s_button.repeat_anchor = now;
            s_button.queued_event = BTN_EVENT_SINGLE_CLICK;
            evt = BTN_EVENT_LONG_PRESS_TICK;
        }
        break;

    case STATE_LONG_PRESS:
        if (release_edge) {
            /* Never generate a click when releasing a long press. */
            s_button.state = STATE_IDLE;
        } else if (raw_pressed && s_button.stable_pressed &&
                   (uint32_t)(now - s_button.repeat_anchor) >= LONG_PRESS_REPEAT_MS) {
            s_button.repeat_anchor = now;
            evt = BTN_EVENT_LONG_PRESS_TICK;
        }
        /* raw_pressed == false: pause repeats immediately, while release debounces. */
        break;

    default:
        s_button.state = STATE_IDLE;
        break;
    }

    if (evt == BTN_EVENT_NONE && s_button.queued_event != BTN_EVENT_NONE) {
        evt = s_button.queued_event;
        s_button.queued_event = BTN_EVENT_NONE;
    }

    return evt;
}

/* --------------------------------- Main --------------------------------- */
void app_main(void)
{
    hardware_init();

    uint8_t counter = 0U;
    display_digit(counter);

    /* Ensure the task really sleeps if FreeRTOS tick frequency is 100 Hz. */
    TickType_t poll_ticks = pdMS_TO_TICKS(BUTTON_POLL_INTERVAL_MS);
    if (poll_ticks == 0U) {
        poll_ticks = 1U;
    }

    while (true) {
        const bool raw_pressed = read_button_pressed();
        const uint32_t now = get_time_ms();
        const button_event_t evt = poll_button_event(raw_pressed, now);

        switch (evt) {
        case BTN_EVENT_SINGLE_CLICK:    
            counter = (uint8_t)((counter + 1) % 10);
            display_digit(counter);
            break;
        case BTN_EVENT_LONG_PRESS_TICK:
            counter = (uint8_t)((counter + 1) % 10);
            display_digit(counter);
            break;
        case BTN_EVENT_DOUBLE_CLICK:
            counter = (uint8_t)((counter + 10 - 1) % 10);
            display_digit(counter);
            break;
        case BTN_EVENT_NONE:
        default:
            break;
        }

        vTaskDelay(poll_ticks);
    }
}
