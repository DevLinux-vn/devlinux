#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "PWM";

/* --- HARDWARE CONSTANTS --- */
#define LED_PIN              GPIO_NUM_15
#define BTN_PIN              GPIO_NUM_16
#define ADC_CHANNEL_POT      ADC_CHANNEL_0 /* GPIO1 */

/* --- LEDC CONSTANTS --- */
#define LEDC_MODE            LEDC_LOW_SPEED_MODE
#define LEDC_TIMER           LEDC_TIMER_0
#define LEDC_CHANNEL         LEDC_CHANNEL_0
#define LEDC_DUTY_RES        LEDC_TIMER_13_BIT
#define LEDC_FREQ_HZ         (5000U)
#define LEDC_DUTY_MAX        ((1U << 13) - 1U) /* 8191 */
#define FADE_TIME_MS         (2000U)

/* --- ADC & BUTTON CONSTANTS --- */
#define ADC_UNIT_USED        ADC_UNIT_1
#define ADC_ATTEN_USED       ADC_ATTEN_DB_12
#define ADC_BITWIDTH         ADC_BITWIDTH_DEFAULT
#define ADC_RAW_MAX          (4095U) /* 12-bit ADC */
#define SAMPLE_COUNT         (16U)
#define DEBOUNCE_US          (25000ULL) /* 25 ms */

typedef enum { MODE_KNOB, MODE_BREATHE } app_mode_t;

static QueueHandle_t btn_queue;

/* --- ISR --- */
static void IRAM_ATTR button_isr(void* arg) {
    static uint64_t last_isr_time = 0;
    uint64_t now = esp_timer_get_time();
    
    if (now - last_isr_time > DEBOUNCE_US) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        uint8_t dummy = 1;
        xQueueSendFromISR(btn_queue, &dummy, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR();
        }
        last_isr_time = now;
    }
}

void app_main(void) {
    /* 1. Init Button Interrupt */
    btn_queue = xQueueCreate(5, sizeof(uint8_t));
    gpio_config_t btn_cfg = {
        .pin_bit_mask = (1ULL << BTN_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    ESP_ERROR_CHECK(gpio_config(&btn_cfg));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BTN_PIN, button_isr, NULL));

    /* 2. Init ADC1 (Potentiometer) */
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t unit_cfg = { .unit_id = ADC_UNIT_USED };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_USED,
        .bitwidth = ADC_BITWIDTH,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_POT, &chan_cfg));

    /* 3. Init LEDC */
    ledc_timer_config_t timer_cfg = {
        .speed_mode      = LEDC_MODE,
        .timer_num       = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz         = LEDC_FREQ_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));

    ledc_channel_config_t channel_cfg = {
        .gpio_num   = LED_PIN,
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL,
        .timer_sel  = LEDC_TIMER,
        .duty       = 0,
        .hpoint     = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_cfg));
    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    /* 4. Main Application Loop */
    app_mode_t current_mode = MODE_KNOB;
    bool fade_up = true;
    uint8_t ev;

    while (1) {
        if (current_mode == MODE_KNOB) {
            /* Check if button was pressed */
            if (xQueueReceive(btn_queue, &ev, 0) == pdTRUE) {
                current_mode = MODE_BREATHE;
                fade_up = true; /* Reset breathe direction */
                continue;
            }

            /* Read ADC and average */
            uint32_t sum = 0;
            int raw = 0;
            for (int i = 0; i < SAMPLE_COUNT; i++) {
                ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_POT, &raw));
                sum += raw;
            }
            uint32_t avg_raw = sum / SAMPLE_COUNT;

            /* Map 12-bit ADC (0-4095) to 13-bit Duty (0-8191) */
            uint32_t duty = (avg_raw * LEDC_DUTY_MAX) / ADC_RAW_MAX;

            /* ledc_set_duty only updates the internal register. 
             * ledc_update_duty applies it to the actual hardware output. */
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

            ESP_LOGI(TAG, "mode=KNOB  raw=%lu  duty=%lu", avg_raw, duty);
            vTaskDelay(pdMS_TO_TICKS(200));

        } else {
            /* MODE_BREATHE */
            uint32_t target_duty = fade_up ? LEDC_DUTY_MAX : 0;
            
            ESP_LOGI(TAG, "mode=BREATHE fade %s -> %lu over %u ms", 
                     fade_up ? "up" : "down", target_duty, FADE_TIME_MS);

            ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, target_duty, FADE_TIME_MS));
            /* Start fade in background (NO_WAIT) so CPU can sleep and listen to the button queue */
            ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT));

            /* Wait exactly the fade duration for a button press */
            if (xQueueReceive(btn_queue, &ev, pdMS_TO_TICKS(FADE_TIME_MS)) == pdTRUE) {
                /* Button pressed during fade -> instantly switch back to KNOB */
                current_mode = MODE_KNOB;
                /* Note: We intentionally leave the LED at its current intermediate duty 
                 * until the KNOB mode reads the ADC on the next loop iteration[cite: 1]. */
            } else {
                /* Timeout reached -> Fade finished naturally, reverse direction */
                fade_up = !fade_up;
            }
        }
    }
}