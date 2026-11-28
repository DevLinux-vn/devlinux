#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_timer.h"

#define BUTTON_PIN      GPIO_NUM_14
#define DEBOUNCE_US     25000LL
#define DOUBLE_CLICK_US 350000LL
#define LONG_PRESS_US   500000LL
#define REPEAT_US       500000LL

// Thanh ghi ESP32-S3. Cac chan LED deu nho hon 32.
#define GPIO_ENABLE_SET (*(volatile uint32_t *)0x60004024UL)
#define GPIO_OUT_SET    (*(volatile uint32_t *)0x60004008UL)
#define GPIO_OUT_CLEAR  (*(volatile uint32_t *)0x6000400CUL)
#define IO_MUX(pin)     (*(volatile uint32_t *)(0x60009004UL + (pin) * 4U))

// Thu tu a, b, c, d, e, f, g; LED common cathode (muc 1 la sang).
static const uint8_t segment_pins[] = {4, 5, 6, 7, 15, 16, 17};
static const uint8_t digit_segments[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static void gpio_init_led(void)
{
    for (unsigned seg = 0; seg < sizeof(segment_pins); seg++)
    {
        const uint8_t pin = segment_pins[seg];
        // GPIO function = 1, drive strength = 1, tat input.
        IO_MUX(pin) = (IO_MUX(pin) & ~((7UL << 12) | (3UL << 10) | (1UL << 9)))
                    | (1UL << 12) | (1UL << 10);
        GPIO_ENABLE_SET = 1UL << pin;
    }
}

static void seven_segment_show(uint8_t digit)
{
    if (digit > 9) return;
    for (unsigned seg = 0; seg < sizeof(segment_pins); seg++)
    {
        if (digit_segments[digit] & (1U << seg))
            GPIO_OUT_SET = 1UL << segment_pins[seg];
        else
            GPIO_OUT_CLEAR = 1UL << segment_pins[seg];
    }
}

static portMUX_TYPE button_mux = portMUX_INITIALIZER_UNLOCKED;
static volatile bool button_pressed = false;
static volatile int64_t press_time_us = 0;
static volatile int64_t release_time_us = 0;

// ISR chi luu trang thai va thoi gian nhan/nha; khong delay hay ghi LED.
static void IRAM_ATTR button_isr_handler(void *arg)
{
    (void)arg;
    portENTER_CRITICAL_ISR(&button_mux);
    const bool pressed = gpio_get_level(BUTTON_PIN) == 0;
    const int64_t now = esp_timer_get_time();
    button_pressed = pressed;
    if (pressed)
    {
        press_time_us = now;
    }
    else
    {
        release_time_us = now;
    }
    portEXIT_CRITICAL_ISR(&button_mux);
}

void app_main(void)
{
    // 1. Khoi tao LED 7 doan va hien thi so 0.
    gpio_init_led();
    uint8_t count = 0;
    seven_segment_show(count);

    // 2. Nut GPIO14 active-low, pull-up; ngat ca nhan va nha.
    const gpio_config_t btn_conf =
    {
        .pin_bit_mask = 1ULL << BUTTON_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&btn_conf));
    // 3. Cai dat dich vu ngat va gan ham xu ly cho nut.
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL));

    bool stable_pressed = false;
    bool long_press = false;
    bool pending_click = false;
    bool second_click = false;
    int64_t click_deadline = 0;
    int64_t repeat_deadline = 0;

    // 4. Lay trang thai da luu boi ISR, chong doi va giai ma cu chi.
    while (1)
    {
        // Lay ban sao nhat quan; volatile khong bao ve du lieu 64-bit.
        portENTER_CRITICAL(&button_mux);
        const bool raw_pressed = button_pressed;
        const int64_t pressed_at = press_time_us;
        const int64_t released_at = release_time_us;
        portEXIT_CRITICAL(&button_mux);
        const int64_t now = esp_timer_get_time();
        const int64_t edge_at = raw_pressed ? pressed_at : released_at;

        // Chong doi khong chan: chi nhan trang thai on dinh du 25 ms.
        if (raw_pressed != stable_pressed && now - edge_at >= DEBOUNCE_US)
        {
            stable_pressed = raw_pressed;
            if (stable_pressed)
            {
                // Canh nhan thu hai phai nam trong cua so double click.
                second_click = pending_click && pressed_at <= click_deadline;
                if (pending_click && !second_click)
                {
                    count = (count + 1U) % 10U;
                    seven_segment_show(count);
                    pending_click = false;
                }
                long_press = false;
                repeat_deadline = pressed_at + LONG_PRESS_US;
            }
            else if (long_press)
            {
                // Nha sau long press khong duoc tinh them click.
                long_press = false;
                second_click = false;
            }
            else if (second_click)
            {
                count = (count + 9U) % 10U;
                seven_segment_show(count);
                pending_click = false;
                second_click = false;
            }
            else
            {
                pending_click = true;
                click_deadline = released_at + DOUBLE_CLICK_US;
            }
        }

        // Long press van lap moi 500 ms du khong co ngat moi.
        if (stable_pressed && raw_pressed && now >= repeat_deadline)
        {
            if (!long_press)
            {
                // Click ngan truoc do va lan giu nay la hai cu chi.
                if (pending_click)
                {
                    count = (count + 1U) % 10U;
                    pending_click = false;
                }
                long_press = true;
                second_click = false;
            }
            count = (count + 1U) % 10U;
            seven_segment_show(count);
            repeat_deadline += REPEAT_US;
        }

        // Cho xac nhan neu canh nhan thu hai dang duoc chong doi.
        if (pending_click && !stable_pressed && now >= click_deadline &&
            !(raw_pressed && pressed_at <= click_deadline))
        {
            count = (count + 1U) % 10U;
            seven_segment_show(count);
            pending_click = false;
        }
        // Nhuong CPU, khong cho bang vong lap ban.
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}