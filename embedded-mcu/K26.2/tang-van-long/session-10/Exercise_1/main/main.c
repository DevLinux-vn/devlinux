/* EXERCISE 1*/
#include <stdbool.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "VMETER";

/* ---- Pin / channel configuration ---------------------------------- */
#define POT_ADC_UNIT     ADC_UNIT_1          
                                                                                                                                     
#define POT_ADC_CHANNEL  ADC_CHANNEL_3       
#define POT_ADC_ATTEN    ADC_ATTEN_DB_12     
                                      
#define POT_ADC_BITWIDTH ADC_BITWIDTH_DEFAULT

#define LED_PIN          GPIO_NUM_15

/* ---- Timing / averaging / alarm configuration ---------------------- */
#define SAMPLE_COUNT     (8U)     
#define REPORT_PERIOD_MS (500U)    
#define ALARM_ON_MV      (2000)    
#define ALARM_OFF_MV     (1800)    

static adc_oneshot_unit_handle_t s_adc_handle;
static adc_cali_handle_t s_cali_handle = NULL;
static bool s_cali_available = false;

static void led_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_set_level(LED_PIN, 0));
}

static void adc_init(void)
{
    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = POT_ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &s_adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = POT_ADC_ATTEN,
        .bitwidth = POT_ADC_BITWIDTH,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(s_adc_handle, POT_ADC_CHANNEL, &chan_cfg));
}

static void adc_cali_init(void)
{
    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id = POT_ADC_UNIT,
        .chan = POT_ADC_CHANNEL,
        .atten = POT_ADC_ATTEN,
        .bitwidth = POT_ADC_BITWIDTH,
    };

    esp_err_t err = adc_cali_create_scheme_curve_fitting(&cali_cfg, &s_cali_handle);
    if (err == ESP_OK) {
        s_cali_available = true;
    } else {
        s_cali_available = false;
        ESP_LOGW(TAG, "ADC calibration unavailable (err=%d); logging raw counts only", err);
    }
}

static bool read_avg_raw(int *avg_raw)
{
    int64_t sum = 0;
    uint32_t ok_count = 0;

    for (uint32_t i = 0; i < SAMPLE_COUNT; i++) {
        int raw = 0;
        esp_err_t err = adc_oneshot_read(s_adc_handle, POT_ADC_CHANNEL, &raw);
        if (err != ESP_OK) {
            continue; 
        }
        sum += raw;
        ok_count++;
    }

    if (ok_count == 0) {
        return false;
    }

    *avg_raw = (int)(sum / ok_count);
    return true;
}

void app_main(void)
{
    led_init();
    adc_init();
    adc_cali_init();

    bool led_on = false;

    while (1) {
        int avg_raw;
        if (!read_avg_raw(&avg_raw)) {
            vTaskDelay(pdMS_TO_TICKS(REPORT_PERIOD_MS));
            continue;
        }
        if (s_cali_available) {
            int mv = 0;
            esp_err_t err = adc_cali_raw_to_voltage(s_cali_handle, avg_raw, &mv);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "raw=%5d  -> %4d mV", avg_raw, mv);

                if (!led_on && mv > ALARM_ON_MV) {
                    led_on = true;
                    gpio_set_level(LED_PIN, 1);
                    ESP_LOGI(TAG, "ALARM ON  (%d mV > %d mV)", mv, ALARM_ON_MV);
                } else if (led_on && mv < ALARM_OFF_MV) {
                    led_on = false;
                    gpio_set_level(LED_PIN, 0);
                    ESP_LOGI(TAG, "ALARM OFF (%d mV < %d mV)", mv, ALARM_OFF_MV);
                }
            } else {
                ESP_LOGW(TAG, "raw=%5d  -> calibration read failed (err=%d)", avg_raw, err);
            }
        } else {
            ESP_LOGI(TAG, "raw=%5d  (no calibration available)", avg_raw);
        }

        vTaskDelay(pdMS_TO_TICKS(REPORT_PERIOD_MS));
    }
}