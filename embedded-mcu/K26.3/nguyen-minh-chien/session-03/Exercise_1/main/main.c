/*
 * Exercise_1 - Button-driven 7-segment counter (register level)
 * Display type: COMMON CATHODE (đổi COMMON_ANODE thành 1 nếu dùng anode chung)
 */
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ---------- Địa chỉ thanh ghi ---------- */
#define DR_REG_GPIO_BASE        (0x60004000UL)
#define DR_REG_IO_MUX_BASE      (0x60009000UL)

#define GPIO_OUT_W1TS_OFFSET    (0x08UL)
#define GPIO_OUT_W1TC_OFFSET    (0x0CUL)
#define GPIO_ENABLE_W1TS_OFFSET (0x24UL)
#define GPIO_IN_OFFSET          (0x3CUL)
#define GPIO_FUNC_OUT_SEL_BASE  (0x554UL)
#define IO_MUX_GPIO0_OFFSET     (0x04UL)

#define GPIO_OUT_W1TS_REG    (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_OUT_W1TS_OFFSET))
#define GPIO_OUT_W1TC_REG    (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_OUT_W1TC_OFFSET))
#define GPIO_ENABLE_W1TS_REG (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_ENABLE_W1TS_OFFSET))
#define GPIO_IN_REG          (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_IN_OFFSET))
#define GPIO_FUNC_OUT_SEL_CFG_REG(n) \
    (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_FUNC_OUT_SEL_BASE + 4UL * (n)))
#define IO_MUX_GPIO_REG(n) \
    (*(volatile uint32_t *)(DR_REG_IO_MUX_BASE + IO_MUX_GPIO0_OFFSET + 4UL * (n)))

/* ---------- Bit/giá trị trong thanh ghi ---------- */
#define MCU_SEL_SHIFT        (12U)
#define MCU_SEL_MASK         (7UL << MCU_SEL_SHIFT)
#define MCU_SEL_GPIO         (1UL << MCU_SEL_SHIFT)   /* Function 1 = GPIO */
#define FUN_IE_BIT           (1UL << 9)
#define FUN_WPU_BIT          (1UL << 8)
#define FUN_WPD_BIT          (1UL << 7)
#define SIG_GPIO_OUT_IDX     (0x100UL)                /* simple GPIO output */

/* ---------- Cấu hình bài ---------- */
#define COMMON_ANODE         (0U)   /* 0 = cathode chung, 1 = anode chung */

#define SEG_COUNT            (7U)
#define BTN_PIN              (14U)

#define DEBOUNCE_MS          (25U)
#define DOUBLE_CLICK_MS      (350U)
#define LONG_PRESS_MS        (800U)
#define REPEAT_PERIOD_MS     (500U)
#define POLL_PERIOD_MS       (10U)
#define DEBOUNCE_TICKS       ((DEBOUNCE_MS + POLL_PERIOD_MS - 1U) / POLL_PERIOD_MS)

#define DIGIT_COUNT          (10U)

static const uint8_t SEG_PINS[SEG_COUNT] = { 4U, 5U, 6U, 7U, 15U, 16U, 17U }; /* a..g */

/* Bit b0..b6 = a..g, bảng cho cathode chung */
static const uint8_t SEGMENT_MAP[DIGIT_COUNT] = {
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U,
    0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU,
};

static uint32_t s_all_seg_mask;
static uint32_t s_high_mask[DIGIT_COUNT];  /* chân cần kéo lên cao cho từng số */

/* ---------- Cấu hình chân ---------- */
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

static void pin_init_input_pullup(uint32_t pin)
{
    uint32_t reg = IO_MUX_GPIO_REG(pin);
    reg &= ~MCU_SEL_MASK;
    reg |= MCU_SEL_GPIO;
    reg |= FUN_IE_BIT;
    reg |= FUN_WPU_BIT;
    reg &= ~FUN_WPD_BIT;
    IO_MUX_GPIO_REG(pin) = reg;
}

/* ---------- Hiển thị ---------- */
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
        /* Điểm khác biệt duy nhất giữa 2 loại LED: đảo cực */
        s_high_mask[d] = COMMON_ANODE ? (s_all_seg_mask & ~lit) : lit;
    }
}

static void display_show(uint32_t digit)
{
    uint32_t high = s_high_mask[digit];
    GPIO_OUT_W1TS_REG = high;                    /* chân cần mức cao */
    GPIO_OUT_W1TC_REG = s_all_seg_mask & ~high;  /* các segment còn lại mức thấp */
}

/* ---------- Nút nhấn ---------- */
static uint32_t button_raw(void)
{
    return (GPIO_IN_REG >> BTN_PIN) & 1UL;   /* 1 = thả, 0 = nhấn */
}

static uint32_t button_debounced(void)
{
    static uint32_t stable = 1U;
    static uint32_t count  = 0U;
    uint32_t raw = button_raw();

    if (raw != stable) {
        if (++count >= DEBOUNCE_TICKS) {
            stable = raw;
            count  = 0U;
        }
    } else {
        count = 0U;
    }
    return stable;
}

/* ---------- Máy trạng thái cử chỉ ---------- */
typedef enum {
    ST_IDLE,
    ST_DOWN,
    ST_REPEAT,
    ST_WAIT_2ND,
    ST_WAIT_RELEASE
} gesture_state_t;

void app_main(void)
{
    for (uint32_t s = 0; s < SEG_COUNT; s++) {
        pin_init_output(SEG_PINS[s]);
    }
    pin_init_input_pullup(BTN_PIN);
    display_build_masks();

    uint32_t counter = 0U;
    display_show(counter);

    gesture_state_t state = ST_IDLE;
    uint32_t now_ms = 0U, t_press = 0U, t_release = 0U, t_repeat = 0U;
    uint32_t prev_btn = 1U;

    TickType_t last_wake = xTaskGetTickCount();

    while (1) {
        uint32_t btn      = button_debounced();
        uint32_t pressed  = (prev_btn == 1U && btn == 0U);
        uint32_t released = (prev_btn == 0U && btn == 1U);
        prev_btn = btn;

        switch (state) {
        case ST_IDLE:
            if (pressed) { t_press = now_ms; state = ST_DOWN; }
            break;

        case ST_DOWN:
            if (released) {
                t_release = now_ms;
                state = ST_WAIT_2ND;              /* click chưa chốt */
            } else if (now_ms - t_press >= LONG_PRESS_MS) {
                counter = (counter + 1U) % DIGIT_COUNT;
                display_show(counter);
                t_repeat = now_ms;
                state = ST_REPEAT;
            }
            break;

        case ST_REPEAT:
            if (released) {
                state = ST_IDLE;                  /* không tính là click */
            } else if (now_ms - t_repeat >= REPEAT_PERIOD_MS) {
                counter = (counter + 1U) % DIGIT_COUNT;
                display_show(counter);
                t_repeat += REPEAT_PERIOD_MS;
            }
            break;

        case ST_WAIT_2ND:
            if (pressed) {                        /* double click: -1 */
                counter = (counter + DIGIT_COUNT - 1U) % DIGIT_COUNT;
                display_show(counter);
                state = ST_WAIT_RELEASE;
            } else if (now_ms - t_release >= DOUBLE_CLICK_MS) {
                counter = (counter + 1U) % DIGIT_COUNT;   /* click đơn: +1 */
                display_show(counter);
                state = ST_IDLE;
            }
            break;

        case ST_WAIT_RELEASE:
            if (released) state = ST_IDLE;
            break;
        }

        now_ms += POLL_PERIOD_MS;
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(POLL_PERIOD_MS));
    }
}
