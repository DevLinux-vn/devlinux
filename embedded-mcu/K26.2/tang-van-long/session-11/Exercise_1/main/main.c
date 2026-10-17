#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "esp_err.h"

/* ------------------------------------------------------------------ */
/* Named constants                                                     */
/* ------------------------------------------------------------------ */
#define BTN_PIN             GPIO_NUM_0      
#define LED_PIN             GPIO_NUM_15     

#define TIMER_RESOLUTION_HZ (1000000U)      
#define TIMER_ALARM_TICKS   (1000ULL)       

#define PRINT_PERIOD_MS     (100U)         
#define LONG_PRESS_MS       (1000U)         
#define LED_HALF_PERIOD_MS  (500U)          

#define BTN_POLL_PERIOD_MS  (5U)            
#define DEBOUNCE_MS         (20U)           
#define DEBOUNCE_STABLE_CNT (DEBOUNCE_MS / BTN_POLL_PERIOD_MS)

static const char *TAG = "WATCH";

/* ------------------------------------------------------------------ */
/* Shared state                                                        */
/* ------------------------------------------------------------------ */

static volatile uint32_t elapsed_ms = 0;
static portMUX_TYPE elapsed_mux = portMUX_INITIALIZER_UNLOCKED;

static volatile bool running = false;

static gptimer_handle_t gptimer = NULL;

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static void format_time(char *buf, size_t len, uint32_t ms)
{
    uint32_t total_s = ms / 1000U;
    uint32_t milli   = ms % 1000U;
    uint32_t minutes = total_s / 60U;
    uint32_t seconds = total_s % 60U;
    snprintf(buf, len, "%02u:%02u.%03u", (unsigned)minutes, (unsigned)seconds, (unsigned)milli);
}

static void check_gptimer_call(esp_err_t err, const char *what)
{
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s failed: %s", what, esp_err_to_name(err));
    }
}

/* ------------------------------------------------------------------ */
/* Timer alarm ISR                                                     */
/* ------------------------------------------------------------------ */

static bool IRAM_ATTR on_alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    portENTER_CRITICAL_ISR(&elapsed_mux);
    elapsed_ms++;
    portEXIT_CRITICAL_ISR(&elapsed_mux);
    return false;
}

static void stopwatch_start(void)
{
    check_gptimer_call(gptimer_start(gptimer), "gptimer_start");
    running = true;
    ESP_LOGI(TAG, "started");
}

static void stopwatch_pause(void)
{
    check_gptimer_call(gptimer_stop(gptimer), "gptimer_stop");
    running = false;

    uint32_t ms = elapsed_ms;
    char buf[16];
    format_time(buf, sizeof(buf), ms);
    ESP_LOGI(TAG, "paused at %s", buf);
}

static void stopwatch_reset(void)
{
    if (running) {
        check_gptimer_call(gptimer_stop(gptimer), "gptimer_stop");
        running = false;
    }
    portENTER_CRITICAL(&elapsed_mux);
    elapsed_ms = 0;
    portEXIT_CRITICAL(&elapsed_mux);

    check_gptimer_call(gptimer_set_raw_count(gptimer, 0), "gptimer_set_raw_count");

    ESP_LOGI(TAG, "reset to 00:00.000");
}

static void button_task(void *arg)
{
    int stable_level = 1;     
    int last_raw     = 1;
    unsigned stable_count = 0;

    bool pressed = false;
    bool long_press_handled = false;
    TickType_t press_start_tick = 0;

    while (1) {
        int raw = gpio_get_level(BTN_PIN);

        if (raw != last_raw) {
            last_raw = raw;
            stable_count = 0;
        } else if (stable_count < DEBOUNCE_STABLE_CNT) {
            stable_count++;
        }

        if (stable_count >= DEBOUNCE_STABLE_CNT && raw != stable_level) {
            stable_level = raw;

            if (stable_level == 0) {
                pressed = true;
                long_press_handled = false;
                press_start_tick = xTaskGetTickCount();
            } else {
                if (pressed && !long_press_handled) {
                    if (running) {
                        stopwatch_pause();
                    } else {
                        stopwatch_start();
                    }
                }
                pressed = false;
            }
        }
        if (pressed && !long_press_handled) {
            uint32_t held_ms = (xTaskGetTickCount() - press_start_tick) * portTICK_PERIOD_MS;
            if (held_ms >= LONG_PRESS_MS) {
                long_press_handled = true;
                stopwatch_reset();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(BTN_POLL_PERIOD_MS));
    }
}

/* ------------------------------------------------------------------ */
/* Print task                                                          */
/* ------------------------------------------------------------------ */

static void print_task(void *arg)
{
    while (1) {
        uint32_t ms = elapsed_ms;
        bool run = running;

        char buf[16];
        format_time(buf, sizeof(buf), ms);

        if (run) {
            ESP_LOGI(TAG, "%s", buf);
        } else {
            ESP_LOGI(TAG, "%s  [paused]", buf);
        }

        bool led_on = run && (((ms / LED_HALF_PERIOD_MS) % 2U) != 0U);
        gpio_set_level(LED_PIN, led_on);

        vTaskDelay(pdMS_TO_TICKS(PRINT_PERIOD_MS));
    }
}

void app_main(void)
{
    gpio_config_t led_cfg = {
        .pin_bit_mask = 1ULL << LED_PIN,
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&led_cfg));
    gpio_set_level(LED_PIN, 0);

    gpio_config_t btn_cfg = {
        .pin_bit_mask = 1ULL << BTN_PIN,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&btn_cfg));

    gptimer_config_t timer_cfg = {
        .clk_src       = GPTIMER_CLK_SRC_DEFAULT,
        .direction     = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION_HZ,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_cfg, &gptimer));

    gptimer_alarm_config_t alarm_cfg = {
        .alarm_count                = TIMER_ALARM_TICKS,
        .reload_count               = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_cfg));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_alarm,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    xTaskCreate(print_task,  "print_task",  4096, NULL, 5, NULL);
    xTaskCreate(button_task, "button_task", 4096, NULL, 6, NULL);
}