/*
 * Exercise_1 (Session 04) - Interrupt-driven 7-segment counter
 * Display: COMMON CATHODE (set COMMON_ANODE to 1 for common anode)
 * Button : GPIO14, internal pull-up, to GND. Driver API + ISR for the button only.
 * Display: register level (copied from Session 03).
 */
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_attr.h"

/* ================= Display: register level (Session 03) ================= */
#define DR_REG_GPIO_BASE        (0x60004000UL)
#define DR_REG_IO_MUX_BASE      (0x60009000UL)

#define GPIO_OUT_W1TS_OFFSET    (0x08UL)
#define GPIO_OUT_W1TC_OFFSET    (0x0CUL)
#define GPIO_ENABLE_W1TS_OFFSET (0x24UL)
#define GPIO_FUNC_OUT_SEL_BASE  (0x554UL)
#define IO_MUX_GPIO0_OFFSET     (0x04UL)

#define GPIO_OUT_W1TS_REG    (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_OUT_W1TS_OFFSET))
#define GPIO_OUT_W1TC_REG    (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_OUT_W1TC_OFFSET))
#define GPIO_ENABLE_W1TS_REG (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_ENABLE_W1TS_OFFSET))
#define GPIO_FUNC_OUT_SEL_CFG_REG(n) \
    (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_FUNC_OUT_SEL_BASE + 4UL * (n)))
#define IO_MUX_GPIO_REG(n) \
    (*(volatile uint32_t *)(DR_REG_IO_MUX_BASE + IO_MUX_GPIO0_OFFSET + 4UL * (n)))

#define MCU_SEL_SHIFT        (12U)
#define MCU_SEL_MASK         (7UL << MCU_SEL_SHIFT)
#define MCU_SEL_GPIO         (1UL << MCU_SEL_SHIFT)
#define FUN_IE_BIT           (1UL << 9)
#define SIG_GPIO_OUT_IDX     (0x100UL)

#define COMMON_ANODE         (0U)
#define SEG_COUNT            (7U)
#define DIGIT_COUNT          (10U)

static const uint8_t SEG_PINS[SEG_COUNT] = { 4U, 5U, 6U, 7U, 15U, 16U, 17U }; /* a..g */
static const uint8_t SEGMENT_MAP[DIGIT_COUNT] = {
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U,
    0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU,
};

static uint32_t s_all_seg_mask;
static uint32_t s_high_mask[DIGIT_COUNT];

static void pin_init_output(uint32_t pin)
{
    uint32_t reg = IO_MUX_GPIO_REG(pin);
    reg &= ~MCU_SEL_MASK;
    reg |= MCU_SEL_GPIO;
    reg &= ~FUN_IE_BIT;
    IO_MUX_GPIO_REG(pin) = reg;

    GPIO_FUNC_OUT_SEL_CFG_REG(pin) = SIG_GPIO_OUT_IDX;
    GPIO_ENABLE_W1TS_REG = (1UL << pin);
}

static void display_build_masks(void)
{
    s_all_seg_mask = 0U;
    for (uint32_t s = 0; s < SEG_COUNT; s++) {
        s_all_seg_mask |= (1UL << SEG_PINS[s]);
    }
    for (uint32_t d = 0; d < DIGIT_COUNT; d++) {
        uint32_t lit = 0U;
        for (uint32_t s = 0; s < SEG_COUNT; s++) {
            if ((SEGMENT_MAP[d] >> s) & 1U) {
                lit |= (1UL << SEG_PINS[s]);
            }
        }
        s_high_mask[d] = COMMON_ANODE ? (s_all_seg_mask & ~lit) : lit;
    }
}

static void display_show(uint32_t digit)
{
    uint32_t high = s_high_mask[digit];
    GPIO_OUT_W1TS_REG = high;
    GPIO_OUT_W1TC_REG = s_all_seg_mask & ~high;
}

/* ================= Button: Driver API + ISR ================= */
#define BTN_GPIO             (GPIO_NUM_14)

#define LOOP_MS              (10U)
#define DEBOUNCE_US          (25000LL)
#define DOUBLE_CLICK_US      (350000LL)
#define LONG_PRESS_US        (800000LL)
#define REPEAT_PERIOD_US     (500000LL)
#define LOOP_US              ((int64_t)LOOP_MS * 1000LL)
/* Extra margin: a 2nd press near the end of the window is confirmed late by debounce + loop */
#define WAIT_2ND_TIMEOUT_US  (DOUBLE_CLICK_US + DEBOUNCE_US + LOOP_US)

static portMUX_TYPE s_btn_mux = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t s_btn_level  = 1U;  /* idle = 1 (pull-up) */
static volatile int64_t  s_press_us   = 0;   /* time of last falling edge */
static volatile int64_t  s_release_us = 0;   /* time of last rising edge  */

/* ISR: read level, timestamp, save state. Nothing else. */
static void IRAM_ATTR button_isr_handler(void *arg)
{
    uint32_t level = (uint32_t)gpio_get_level(BTN_GPIO);
    int64_t  now   = esp_timer_get_time();

    portENTER_CRITICAL_ISR(&s_btn_mux);
    s_btn_level = level;
    if (level == 0U) {
        s_press_us = now;
    } else {
        s_release_us = now;
    }
    portEXIT_CRITICAL_ISR(&s_btn_mux);
}

static void button_init(void)
{
    gpio_config_t conf = {
        .pin_bit_mask = (1ULL << BTN_GPIO),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_ANYEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&conf));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BTN_GPIO, button_isr_handler, NULL));
}

/* Consistent snapshot of all three shared fields */
static void button_snapshot(uint32_t *level, int64_t *press, int64_t *release)
{
    portENTER_CRITICAL(&s_btn_mux);
    *level   = s_btn_level;
    *press   = s_press_us;
    *release = s_release_us;
    portEXIT_CRITICAL(&s_btn_mux);
}

/* ================= Gesture decoding ================= */
typedef enum {
    ST_IDLE,
    ST_DOWN,
    ST_REPEAT,
    ST_WAIT_2ND,
    ST_WAIT_RELEASE
} gesture_state_t;

static uint32_t counter;

static void counter_inc(void)
{
    counter = (counter + 1U) % DIGIT_COUNT;
    display_show(counter);
}

static void counter_dec(void)
{
    counter = (counter + DIGIT_COUNT - 1U) % DIGIT_COUNT;
    display_show(counter);
}

void app_main(void)
{
    for (uint32_t s = 0; s < SEG_COUNT; s++) {
        pin_init_output(SEG_PINS[s]);
    }
    display_build_masks();
    counter = 0U;
    display_show(counter);

    button_init();

    gesture_state_t state = ST_IDLE;
    uint32_t stable = 1U;                 /* debounced state */
    int64_t t_press = 0, t_release = 0, next_repeat = 0;

    while (1) {
        uint32_t level;
        int64_t press_us, release_us;
        button_snapshot(&level, &press_us, &release_us);
        int64_t now = esp_timer_get_time();

        /* Debounce: accept a new level only after it has been quiet long enough */
        int64_t last_edge = level ? release_us : press_us;
        bool pressed = false, released = false;
        if (level != stable && (now - last_edge) >= DEBOUNCE_US) {
            stable = level;
            if (stable == 0U) pressed = true;
            else              released = true;
        }

        switch (state) {
        case ST_IDLE:
            if (pressed) {
                t_press = press_us;
                state = ST_DOWN;
            }
            break;

        case ST_DOWN:
            if (released) {
                t_release = release_us;
                state = ST_WAIT_2ND;                 /* click not committed yet */
            } else if (level == 0U && (now - t_press) >= LONG_PRESS_US) {
                counter_inc();                       /* +1 on entering long press */
                next_repeat = now + REPEAT_PERIOD_US;
                state = ST_REPEAT;
            }
            break;

        case ST_REPEAT:
            if (released) {
                state = ST_IDLE;                     /* no extra click */
            } else if (level == 0U && now >= next_repeat) {
                counter_inc();
                next_repeat += REPEAT_PERIOD_US;
            }
            break;

        case ST_WAIT_2ND:
            if (pressed) {
                if ((press_us - t_release) <= DOUBLE_CLICK_US) {
                    counter_dec();                   /* double click: -1 */
                    state = ST_WAIT_RELEASE;
                } else {
                    counter_inc();                   /* too late: commit old click, new press */
                    t_press = press_us;
                    state = ST_DOWN;
                }
            } else if ((now - t_release) >= WAIT_2ND_TIMEOUT_US) {
                counter_inc();                       /* single click: +1 */
                state = ST_IDLE;
            }
            break;

        case ST_WAIT_RELEASE:
            if (released) state = ST_IDLE;
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(LOOP_MS));
    }
}