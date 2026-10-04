#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define BTN_PIN             GPIO_NUM_0      /* BOOT button, pressed = LOW     */
#define SENSOR_PERIOD_MS    (200U)
#define REPORT_PERIOD_MS    (1000U)
#define TWDT_TIMEOUT_MS     (3000U)
#define RESET_MAGIC         (0xC0FFEE42U)

#define SENSOR_TASK_NAME    "sensor"
#define REPORT_TASK_NAME    "report"
#define TASK_STACK_BYTES    (4096U)
#define SENSOR_TASK_PRIO    (5U)
#define REPORT_TASK_PRIO    (5U)

#define TWDT_IDLE_CORE_MASK (0U)

static const char *TAG = "WDT";

/* --------------------- Survive a watchdog reset (not power-on) ------------ */
RTC_NOINIT_ATTR static uint32_t wdt_magic;
RTC_NOINIT_ATTR static uint32_t wdt_reset_count;

/* ------------------------------ Shared state ------------------------------ */
static volatile bool s_hang_requested = false;   
static SemaphoreHandle_t s_never_given_sem = NULL; 

/* -------------------------------- Helpers --------------------------------- */
static const char *reset_reason_name(esp_reset_reason_t reason)
{
    switch (reason)
    {
    case ESP_RST_POWERON:   return "POWERON";
    case ESP_RST_EXT:       return "EXT";
    case ESP_RST_SW:        return "SW";
    case ESP_RST_PANIC:     return "PANIC";
    case ESP_RST_INT_WDT:   return "INT_WDT";
    case ESP_RST_TASK_WDT:  return "TASK_WDT";
    case ESP_RST_WDT:       return "WDT";
    case ESP_RST_DEEPSLEEP: return "DEEPSLEEP";
    case ESP_RST_BROWNOUT:  return "BROWNOUT";
    case ESP_RST_SDIO:      return "SDIO";
    case ESP_RST_UNKNOWN:   return "UNKNOWN";
    default:                return "OTHER";
    }
}

static bool is_watchdog_reset(esp_reset_reason_t reason)
{
    return (reason == ESP_RST_PANIC) ||
           (reason == ESP_RST_TASK_WDT) ||
           (reason == ESP_RST_INT_WDT) ||
           (reason == ESP_RST_WDT);
}

/* BOOT button ISR: only sets the flag. No logging, no blocking here. */
static void IRAM_ATTR boot_button_isr(void *arg)
{
    (void)arg;
    s_hang_requested = true;
}

/* --------------------------------- Tasks ---------------------------------- */
static void sensor_task(void *arg)
{
    (void)arg;
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));         

    for (;;)
    {
        ESP_LOGI(TAG, "sensor tick");
        ESP_ERROR_CHECK(esp_task_wdt_reset());        
        vTaskDelay(pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}

static void report_task(void *arg)
{
    (void)arg;
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));        

    for (;;)
    {
        if (s_hang_requested)
        {
            ESP_LOGI(TAG, "BOOT pressed -> report will hang");
            (void)xSemaphoreTake(s_never_given_sem, portMAX_DELAY);
        }

        ESP_LOGI(TAG, "report: all good");
        ESP_ERROR_CHECK(esp_task_wdt_reset());        
        vTaskDelay(pdMS_TO_TICKS(REPORT_PERIOD_MS));
    }
}

/* -------------------------------- app_main -------------------------------- */
void app_main(void)
{
    const esp_reset_reason_t reason = esp_reset_reason();

    if ((wdt_magic != RESET_MAGIC) || (reason == ESP_RST_POWERON))
    {
        wdt_reset_count = 0U;
        wdt_magic       = RESET_MAGIC;
    }

    if (is_watchdog_reset(reason))
    {
        wdt_reset_count++;
    }

    ESP_LOGI(TAG, "boot: reason=%s  watchdog_resets=%" PRIu32,
             reset_reason_name(reason), wdt_reset_count);

    const esp_task_wdt_config_t twdt_cfg = {
        .timeout_ms     = TWDT_TIMEOUT_MS,
        .idle_core_mask = TWDT_IDLE_CORE_MASK,
        .trigger_panic  = true,
    };

    esp_err_t err = esp_task_wdt_reconfigure(&twdt_cfg);
    if (err == ESP_ERR_INVALID_STATE)
    {
        ESP_LOGW(TAG, "TWDT not initialised, calling esp_task_wdt_init()");
        err = esp_task_wdt_init(&twdt_cfg);
    }
    ESP_ERROR_CHECK(err);

    s_never_given_sem = xSemaphoreCreateBinary();
    configASSERT(s_never_given_sem != NULL);

    const gpio_config_t btn_cfg = {
        .pin_bit_mask = (1ULL << BTN_PIN),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_NEGEDGE,            
    };
    ESP_ERROR_CHECK(gpio_config(&btn_cfg));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BTN_PIN, boot_button_isr, NULL));

    BaseType_t ok;
    ok = xTaskCreate(sensor_task, SENSOR_TASK_NAME, TASK_STACK_BYTES, NULL,
                     SENSOR_TASK_PRIO, NULL);
    configASSERT(ok == pdPASS);
    ok = xTaskCreate(report_task, REPORT_TASK_NAME, TASK_STACK_BYTES, NULL,
                     REPORT_TASK_PRIO, NULL);
    configASSERT(ok == pdPASS);
}