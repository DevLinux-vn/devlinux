/* ESP32-S3 DevKitC-1 v1.0; COMMON CATHODE 7-segment, common pin to GND.
 * a..g: GPIO4,5,6,7,15,16,17. Button: GPIO14 to GND, internal pull-up.
 * Display: register-level only. Button level is read ONLY inside the ISR.
 */
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_io_reg.h"

#define SEG_A_PIN                 4U
#define SEG_B_PIN                 5U
#define SEG_C_PIN                 6U
#define SEG_D_PIN                 7U
#define SEG_E_PIN                15U
#define SEG_F_PIN                16U
#define SEG_G_PIN                17U
#define BTN_PIN                  GPIO_NUM_14
#define BTN_INPUT_MASK           (UINT32_C(1) << BTN_PIN)
#define DISPLAY_COMMON_ANODE      0
#define SEGMENT_COUNT             7U
#define DIGIT_COUNT              10U
#define GPIO_REGISTER_STRIDE      4U
#define GPIO_DRIVE_STRENGTH        2U
#define DEBOUNCE_US           INT64_C(25000)
#define DOUBLE_CLICK_US      INT64_C(350000)
#define LONG_PRESS_US        INT64_C(800000)
#define REPEAT_PERIOD_US     INT64_C(500000)
#define BTN_QUEUE_SIZE           64U
#define GESTURE_STACK_BYTES    4096U
#define GESTURE_PRIORITY          8U
#define US_PER_SECOND       INT64_C(1000000)
#define NO_DEADLINE              INT64_MAX

static const char *TAG = "BTN";
static const uint8_t SEGMENT_MAP[DIGIT_COUNT] = {
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U,
    0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU,
};
static const uint8_t SEGMENT_PINS[SEGMENT_COUNT] = {
    SEG_A_PIN, SEG_B_PIN, SEG_C_PIN, SEG_D_PIN,
    SEG_E_PIN, SEG_F_PIN, SEG_G_PIN,
};
static const uintptr_t SEGMENT_MUX[SEGMENT_COUNT] = {
    IO_MUX_GPIO4_REG, IO_MUX_GPIO5_REG, IO_MUX_GPIO6_REG, IO_MUX_GPIO7_REG,
    IO_MUX_GPIO15_REG, IO_MUX_GPIO16_REG, IO_MUX_GPIO17_REG,
};
static const uintptr_t SEGMENT_RTC_MUX[SEGMENT_COUNT] = {
    RTC_IO_TOUCH_PAD4_REG, RTC_IO_TOUCH_PAD5_REG,
    RTC_IO_TOUCH_PAD6_REG, RTC_IO_TOUCH_PAD7_REG,
    RTC_IO_XTAL_32P_PAD_REG, RTC_IO_XTAL_32N_PAD_REG, RTC_IO_PAD_DAC1_REG,
};
static const uint32_t SEGMENT_RTC_MASK[SEGMENT_COUNT] = {
    RTC_IO_TOUCH_PAD4_MUX_SEL, RTC_IO_TOUCH_PAD5_MUX_SEL,
    RTC_IO_TOUCH_PAD6_MUX_SEL, RTC_IO_TOUCH_PAD7_MUX_SEL,
    RTC_IO_X32P_MUX_SEL, RTC_IO_X32N_MUX_SEL, RTC_IO_PDAC1_MUX_SEL,
};

static inline uint32_t reg_read(uintptr_t address)
{
    return *(volatile uint32_t *)address;
}

static inline void reg_write(uintptr_t address, uint32_t value)
{
    *(volatile uint32_t *)address = value;
}

static uint32_t segment_pin_mask(void)
{
    uint32_t mask = 0U;
    for (unsigned i = 0U; i < SEGMENT_COUNT; ++i) {
        mask |= UINT32_C(1) << SEGMENT_PINS[i];
    }
    return mask;
}

static void display_digit(unsigned digit)
{
    uint32_t on_mask = 0U;
    for (unsigned i = 0U; i < SEGMENT_COUNT; ++i) {
        if ((SEGMENT_MAP[digit] & (UINT32_C(1) << i)) != 0U) {
            on_mask |= UINT32_C(1) << SEGMENT_PINS[i];
        }
    }
    const uint32_t all = segment_pin_mask();
    const uint32_t high = DISPLAY_COMMON_ANODE ? (all ^ on_mask) : on_mask;
    /* W1TC/W1TS affect only the seven segment pins, without a shared OUT RMW. */
    reg_write(GPIO_OUT_W1TC_REG, all & ~high);
    reg_write(GPIO_OUT_W1TS_REG, high);
}

static void display_init(void)
{
    const uint32_t all = segment_pin_mask();
    reg_write(GPIO_ENABLE_W1TC_REG, all);
    for (unsigned i = 0U; i < SEGMENT_COUNT; ++i) {
        reg_write(SEGMENT_RTC_MUX[i],
                  reg_read(SEGMENT_RTC_MUX[i]) & ~SEGMENT_RTC_MASK[i]);
        uint32_t mux = reg_read(SEGMENT_MUX[i]);
        mux &= ~(MCU_SEL_M | FUN_IE | FUN_PU | FUN_PD | FUN_DRV_M | SLP_SEL);
        mux |= (PIN_FUNC_GPIO << MCU_SEL_S) | (GPIO_DRIVE_STRENGTH << FUN_DRV_S);
        reg_write(SEGMENT_MUX[i], mux);
        reg_write(GPIO_FUNC0_OUT_SEL_CFG_REG + SEGMENT_PINS[i] * GPIO_REGISTER_STRIDE,
                  SIG_GPIO_OUT_IDX | GPIO_FUNC0_OEN_SEL);
    }
    display_digit(0U);
    reg_write(GPIO_ENABLE_W1TS_REG, all);
}

typedef struct {
    int64_t timestamp_us;
    bool is_press;
} btn_event_t;

static QueueHandle_t btn_queue;
/* Single ISR writer; the task only reads the aligned counter, never clears it. */
static volatile uint32_t dropped_edges;

static void IRAM_ATTR button_isr(void *arg)
{
    (void)arg;
    const btn_event_t event = {
        .timestamp_us = esp_timer_get_time(),
        /* IDF 5.2 gpio_get_level() is in flash; this single MMIO read is ISR-safe. */
        .is_press = (*(volatile const uint32_t *)GPIO_IN_REG & BTN_INPUT_MASK) == 0U,
    };
    BaseType_t wake_task = pdFALSE;
    if (xQueueSendFromISR(btn_queue, &event, &wake_task) != pdTRUE) {
        ++dropped_edges;
    }
    if (wake_task == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/* Pure event/time state machine: no GPIO reads, no task APIs. */
typedef enum {
    GESTURE_CLICK, GESTURE_DOUBLE, GESTURE_LONG_START,
    GESTURE_LONG_REPEAT, GESTURE_LONG_END,
} gesture_action_t;

typedef struct {
    bool raw_pressed;
    bool stable_pressed;
    bool pending_click;
    bool second_press;
    bool long_active;
    int64_t raw_since;
    int64_t press_since;
    int64_t click_deadline;
    int64_t repeat_deadline;
} gesture_state_t;

static unsigned digit;

static void emit_gesture(gesture_action_t action)
{
    if (action == GESTURE_LONG_END) {
        ESP_LOGI(TAG, "LONG end");
        return;
    }
    digit = action == GESTURE_DOUBLE
        ? (digit + DIGIT_COUNT - 1U) % DIGIT_COUNT
        : (digit + 1U) % DIGIT_COUNT;
    display_digit(digit);
    static const char *const names[] = {"CLICK", "DOUBLE", "LONG start", "LONG rep"};
    ESP_LOGI(TAG, "%-10s -> %u", names[action], digit);
}

static void stable_edge(gesture_state_t *s, bool pressed, int64_t edge_time)
{
    s->stable_pressed = pressed;
    if (pressed) {
        s->press_since = edge_time;
        s->long_active = false;
        s->second_press = s->pending_click && edge_time <= s->click_deadline;
        return;
    }
    if (s->long_active) {
        emit_gesture(GESTURE_LONG_END);
        s->long_active = false;
        s->second_press = false;
    } else if (s->second_press) {
        emit_gesture(GESTURE_DOUBLE);
        s->pending_click = false;
        s->second_press = false;
    } else {
        s->pending_click = true;
        s->click_deadline = edge_time + DOUBLE_CLICK_US;
    }
}

static int64_t next_deadline(const gesture_state_t *s)
{
    int64_t next = NO_DEADLINE;
    if (s->raw_pressed != s->stable_pressed) {
        next = s->raw_since + DEBOUNCE_US;
    }
    if (s->stable_pressed && s->raw_pressed) {
        const int64_t held_due = s->long_active
            ? s->repeat_deadline : s->press_since + LONG_PRESS_US;
        if (held_due < next) {
            next = held_due;
        }
    }
    /* A second press just before the limit must first pass debounce. */
    const bool candidate_second = s->raw_pressed && !s->stable_pressed
        && s->raw_since <= s->click_deadline;
    if (s->pending_click && !s->second_press && !candidate_second
        && s->click_deadline < next) {
        next = s->click_deadline;
    }
    return next;
}

static void advance_time(gesture_state_t *s, int64_t now)
{
    for (;;) {
        const int64_t due = next_deadline(s);
        if (due == NO_DEADLINE || due > now) {
            return;
        }
        if (s->raw_pressed != s->stable_pressed
            && s->raw_since + DEBOUNCE_US == due) {
            stable_edge(s, s->raw_pressed, s->raw_since);
        } else if (s->stable_pressed && s->raw_pressed
                   && (s->long_active ? s->repeat_deadline
                                     : s->press_since + LONG_PRESS_US) == due) {
            if (!s->long_active) {
                /* A click followed by a hold is a click plus a hold, not a double. */
                if (s->pending_click) {
                    emit_gesture(GESTURE_CLICK);
                    s->pending_click = false;
                }
                s->second_press = false;
                s->long_active = true;
                emit_gesture(GESTURE_LONG_START);
                s->repeat_deadline = due + REPEAT_PERIOD_US;
            } else {
                emit_gesture(GESTURE_LONG_REPEAT);
                s->repeat_deadline += REPEAT_PERIOD_US;
            }
        } else {
            s->pending_click = false;
            emit_gesture(GESTURE_CLICK);
        }
    }
}

static void receive_edge(gesture_state_t *s, const btn_event_t *event)
{
    advance_time(s, event->timestamp_us);
    if (event->is_press != s->raw_pressed) {
        s->raw_pressed = event->is_press;
        s->raw_since = event->timestamp_us;
    }
    advance_time(s, event->timestamp_us);
}

static TickType_t wait_ticks(const gesture_state_t *s)
{
    const int64_t deadline = next_deadline(s);
    if (deadline == NO_DEADLINE) {
        return portMAX_DELAY;
    }
    const int64_t remaining = deadline - esp_timer_get_time();
    if (remaining <= 0) {
        return 0;
    }
    const int64_t tick_us = US_PER_SECOND / configTICK_RATE_HZ;
    return (TickType_t)((remaining + tick_us - 1) / tick_us);
}

static void gesture_task(void *arg)
{
    (void)arg;
    gesture_state_t state = {0};
    uint32_t seen_drops = 0U;
    bool resynchronizing = false;
    btn_event_t event;
    for (;;) {
        const bool received = xQueueReceive(btn_queue, &event, wait_ticks(&state)) == pdTRUE;
        const uint32_t lost = dropped_edges;
        if (lost != seen_drops) {
            seen_drops = lost;
            state = (gesture_state_t){0};
            resynchronizing = true;
            xQueueReset(btn_queue);
            ESP_LOGW(TAG, "Button queue overflow: waiting for a release edge");
            continue;
        }
        if (received) {
            do {
                if (resynchronizing) {
                    if (!event.is_press) {
                        resynchronizing = false;
                    }
                } else {
                    receive_edge(&state, &event);
                }
            } while (xQueueReceive(btn_queue, &event, 0) == pdTRUE);
        }
        /* Drain timestamped edges first; a queued release must cancel repeats. */
        advance_time(&state, esp_timer_get_time());
    }
}

void app_main(void)
{
    display_init();
    btn_queue = xQueueCreate(BTN_QUEUE_SIZE, sizeof(btn_event_t));
    ESP_ERROR_CHECK(btn_queue != NULL ? ESP_OK : ESP_ERR_NO_MEM);
    const gpio_config_t button_config = {
        .pin_bit_mask = UINT64_C(1) << BTN_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&button_config));
    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_IRAM));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BTN_PIN, button_isr, NULL));
    ESP_ERROR_CHECK(xTaskCreate(gesture_task, "gesture_task", GESTURE_STACK_BYTES,
                               NULL, GESTURE_PRIORITY, NULL) == pdPASS
                    ? ESP_OK : ESP_ERR_NO_MEM);
    ESP_LOGI(TAG, "Ready: click +1, double -1, hold after 800 ms then every 500 ms");
}
