#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#define LED_PIN      GPIO_NUM_15
#define BTN_PIN      GPIO_NUM_16

#define LEDC_MODE    LEDC_LOW_SPEED_MODE      /* the only mode on this chip */
#define LEDC_TIMER   LEDC_TIMER_0
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FREQ_HZ (5000U)                  /* 5000 * 2^13 = 40.96 MHz, fits the 80 MHz source */
#define LEDC_DUTY_MAX ((1U << 13) - 1U)

#define FADE_TIME_MS  (2000U)
#define FADE_POLL_MS  (20U)                   /* how often the fade wait re-checks the mode */

#define ADC_UNIT_USED    ADC_UNIT_1
#define ADC_CHANNEL_POT  ADC_CHANNEL_0
#define ADC_ATTEN_USED   ADC_ATTEN_DB_12      /* roughly 0..3.1 V at the pin */
#define ADC_BITWIDTH     ADC_BITWIDTH_DEFAULT /* 12 bit on the S3 -> 0..4095 */
#define ADC_FULL_SCALE   (4095U)
#define ADC_SAMPLE_COUNT (16U)

#define KNOB_PERIOD_MS   (50U)
#define LOG_PERIOD_MS    (200U)
#define DEBOUNCE_MS      (50U)
#define BTN_QUEUE_DEPTH  (16U)

static const char *TAG = "PWM";

typedef enum {
    MODE_KNOB,
    MODE_BREATHE,
} led_mode_t;

typedef struct {
    int64_t time_us;
    bool pressed;
} btn_event_t;

static volatile led_mode_t mode = MODE_KNOB;

static adc_oneshot_unit_handle_t adc_handle;
static QueueHandle_t btn_queue;
static SemaphoreHandle_t fade_done;

/* ------------------------------------------------------------------ */
/* LEDC                                                                */
/* ------------------------------------------------------------------ */

static bool IRAM_ATTR on_fade_done(const ledc_cb_param_t *param, void *user_arg)
{
    BaseType_t woken = pdFALSE;

    (void)user_arg;
    if (param->event == LEDC_FADE_END_EVT) {
        xSemaphoreGiveFromISR(fade_done, &woken);
    }

    return woken == pdTRUE;
}

static void pwm_setup(void)
{
    const ledc_timer_config_t timer_cfg = {
        .speed_mode      = LEDC_MODE,
        .timer_num       = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz         = LEDC_FREQ_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));

    const ledc_channel_config_t channel_cfg = {
        .gpio_num   = LED_PIN,
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL,
        .timer_sel  = LEDC_TIMER,
        .duty       = 0,
        .hpoint     = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_cfg));

    fade_done = xSemaphoreCreateBinary();
    if (fade_done == NULL) {
        ESP_LOGE(TAG, "no memory for the fade semaphore");
        abort();
    }

    /* The fade engine runs from its own interrupt, so the handler is
     * installed once here and only signals us when a fade has finished. */
    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    ledc_cbs_t fade_cbs = {
        .fade_cb = on_fade_done,
    };
    ESP_ERROR_CHECK(ledc_cb_register(LEDC_MODE, LEDC_CHANNEL, &fade_cbs, NULL));
}

/* Starts one fade and waits for its end interrupt. Returns false without
 * starting one if the mode already changed, or after cancelling the fade when
 * the mode changes mid-way. */
static bool fade_and_wait(uint32_t target_duty)
{
    if (mode != MODE_BREATHE) {
        return false;
    }

    xSemaphoreTake(fade_done, 0);   /* drop a stale signal from the last fade */

    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, target_duty, FADE_TIME_MS));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT));

    ESP_LOGI(TAG, "mode=BREATHE  fade %-4s -> %u over %u ms",
             target_duty == 0 ? "down" : "up", (unsigned)target_duty, FADE_TIME_MS);

    while (mode == MODE_BREATHE) {
        if (xSemaphoreTake(fade_done, pdMS_TO_TICKS(FADE_POLL_MS)) == pdTRUE) {
            return true;
        }
    }

    ledc_fade_stop(LEDC_MODE, LEDC_CHANNEL);
    return false;
}

/* ------------------------------------------------------------------ */
/* ADC                                                                 */
/* ------------------------------------------------------------------ */

static void adc_setup(void)
{
    const adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT_USED,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

    const adc_oneshot_chan_cfg_t chan_cfg = {
        .atten    = ADC_ATTEN_USED,
        .bitwidth = ADC_BITWIDTH,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_POT, &chan_cfg));
}

static uint32_t pot_read_raw(void)
{
    uint32_t sum = 0;

    for (int i = 0; i < ADC_SAMPLE_COUNT; i++) {
        int raw = 0;

        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_POT, &raw));
        sum += (uint32_t)raw;
    }

    return sum / ADC_SAMPLE_COUNT;
}

/* The ADC counts to 4095 and the duty register counts to 8191; scale the
 * raw reading onto the duty range instead of assuming they match. */
static uint32_t duty_from_raw(uint32_t raw)
{
    if (raw > ADC_FULL_SCALE) {
        raw = ADC_FULL_SCALE;
    }

    return raw * LEDC_DUTY_MAX / ADC_FULL_SCALE;
}

/* ------------------------------------------------------------------ */
/* Button                                                              */
/* ------------------------------------------------------------------ */

static void IRAM_ATTR button_isr(void *arg)
{
    BaseType_t woken = pdFALSE;
    const btn_event_t ev = {
        .time_us = esp_timer_get_time(),
        .pressed = (gpio_get_level(BTN_PIN) == 0),
    };

    (void)arg;
    xQueueSendFromISR(btn_queue, &ev, &woken);
    if (woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void button_setup(void)
{
    btn_queue = xQueueCreate(BTN_QUEUE_DEPTH, sizeof(btn_event_t));
    if (btn_queue == NULL) {
        ESP_LOGE(TAG, "no memory for the button queue");
        abort();
    }

    const gpio_config_t btn_cfg = {
        .pin_bit_mask = 1ULL << BTN_PIN,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_ANYEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&btn_cfg));

    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_ERROR_CHECK(err);
    }
    ESP_ERROR_CHECK(gpio_isr_handler_add(BTN_PIN, button_isr, NULL));
}

static void button_task(void *arg)
{
    btn_event_t ev;
    int64_t last_press_us = -(int64_t)DEBOUNCE_MS * 1000;

    (void)arg;
    while (true) {
        if (xQueueReceive(btn_queue, &ev, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        /* Only the press edge matters, and bounce produces a burst of them. */
        if (!ev.pressed) {
            continue;
        }
        if (ev.time_us - last_press_us < (int64_t)DEBOUNCE_MS * 1000) {
            continue;
        }
        last_press_us = ev.time_us;

        mode = (mode == MODE_KNOB) ? MODE_BREATHE : MODE_KNOB;
        ESP_LOGI(TAG, "button -> %s", mode == MODE_KNOB ? "KNOB" : "BREATHE");
    }
}

/* ------------------------------------------------------------------ */
/* Modes                                                               */
/* ------------------------------------------------------------------ */

static void run_knob(void)
{
    int64_t next_log_us = 0;

    while (mode == MODE_KNOB) {
        const uint32_t raw  = pot_read_raw();
        const uint32_t duty = duty_from_raw(raw);

        /* set_duty only writes the register; update_duty latches it into
         * the PWM generator at the next period boundary. */
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

        if (esp_timer_get_time() >= next_log_us) {
            ESP_LOGI(TAG, "mode=KNOB  raw=%4u  duty=%4u", (unsigned)raw, (unsigned)duty);
            next_log_us = esp_timer_get_time() + (int64_t)LOG_PERIOD_MS * 1000;
        }

        vTaskDelay(pdMS_TO_TICKS(KNOB_PERIOD_MS));
    }
}

static void run_breathe(void)
{
    while (mode == MODE_BREATHE) {
        if (!fade_and_wait(LEDC_DUTY_MAX) || !fade_and_wait(0)) {
            break;
        }
    }
}

void app_main(void)
{
    pwm_setup();
    adc_setup();
    button_setup();

    if (xTaskCreate(button_task, "button", 3072, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "cannot create the button task");
        return;
    }

    while (true) {
        if (mode == MODE_KNOB) {
            run_knob();
        } else {
            run_breathe();
        }
    }
}
