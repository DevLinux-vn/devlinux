#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "driver/gptimer.h"
#include "esp_task_wdt.h"
#include "esp_log.h"

/* --- TIMING CONSTANTS --- */
#define SOFT_TIMER_PERIOD_US (1000000ULL) /* 1 s */
#define GPTIMER_RESOLUTION   (1000000U)   /* 1 MHz -> 1 tick == 1 us */
#define GPTIMER_ALARM_TICKS  (250000ULL)  /* 250 ms at that resolution */
#define WDT_TIMEOUT_MS       (5000U)      /* 5 seconds */
#define STALL_DURATION_MS    (8000U)      /* 8 seconds, deliberately longer than WDT */

static const char *TAG = "TIMERS";
static volatile uint32_t hw_alarm_count = 0; /* Volatile because it is shared between ISR and Task */
static uint32_t uptime_sec = 0;

/* --- PART 1: esp_timer CALLBACK --- */
static void soft_timer_cb(void* arg) {
    uptime_sec++;
    ESP_LOGI(TAG, "uptime = %lu s", uptime_sec);
}

/* --- PART 2: gptimer CALLBACK (ISR Context) --- */
static bool IRAM_ATTR hw_timer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_ctx) {
    hw_alarm_count++;
    return false; /* Return false because we do not need to yield/wake a high-priority task here */
}

/* --- PART 3: WATCHDOG STALL FUNCTIONS --- */

/* 
 * STALLING VERSION (Commented out per requirements)
 * 
static void stall_cpu_crashing(uint32_t duration_ms) {
    int64_t end_time = esp_timer_get_time() + (duration_ms * 1000ULL);
    while (esp_timer_get_time() < end_time) {
        // Genuine busy-wait. CPU is hogged, IDLE task starves, Watchdog bites.
    }
}
*/

/* FIX 1: Feed the watchdog inside the long loop */
static void stall_cpu_feed_wdt(uint32_t duration_ms) {
    int64_t end_time = esp_timer_get_time() + (duration_ms * 1000ULL);
    while (esp_timer_get_time() < end_time) {
        esp_task_wdt_reset(); 
    }
}

/* FIX 2: Yield the CPU (The proper fix) */
static void stall_cpu_yield(uint32_t duration_ms) {
    vTaskDelay(pdMS_TO_TICKS(duration_ms)); 
}

/* --- MAIN APPLICATION --- */
void app_main(void) {
    /* 1. Init Software Timer */
    esp_timer_handle_t soft_handle;
    const esp_timer_create_args_t soft_args = {
        .callback = soft_timer_cb,
        .name = "uptime"
    };
    ESP_ERROR_CHECK(esp_timer_create(&soft_args, &soft_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(soft_handle, SOFT_TIMER_PERIOD_US));

    /* 2. Init Hardware Timer (Strict ordering applied) */
    gptimer_handle_t hw_timer = NULL;
    gptimer_config_t hw_cfg = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = GPTIMER_RESOLUTION,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&hw_cfg, &hw_timer));

    gptimer_alarm_config_t alarm_cfg = {
        .alarm_count = GPTIMER_ALARM_TICKS,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(hw_timer, &alarm_cfg));

    gptimer_event_callbacks_t cbs = { .on_alarm = hw_timer_cb };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(hw_timer, &cbs, NULL));
    ESP_ERROR_CHECK(gptimer_enable(hw_timer));
    ESP_ERROR_CHECK(gptimer_start(hw_timer));

    /* 3. Task Watchdog Configuration */
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = WDT_TIMEOUT_MS,
        .idle_core_mask = (1 << 0) | (1 << 1),
        .trigger_panic = true,
    };
    
    esp_err_t err = esp_task_wdt_init(&wdt_config);
    if (err == ESP_ERR_INVALID_STATE) {
        ESP_ERROR_CHECK(esp_task_wdt_reconfigure(&wdt_config));
    } else {
        ESP_ERROR_CHECK(err);
    }
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL)); 

    /* Trigger the fixes */
    // stall_cpu_crashing(STALL_DURATION_MS); /* Triggers Reboot */
    stall_cpu_feed_wdt(STALL_DURATION_MS);    /* Fix 1 */
    stall_cpu_yield(STALL_DURATION_MS);       /* Fix 2 */

    /* 4. Infinite Loop */
    while (1) {
        ESP_LOGI(TAG, "hw_alarm_count = %lu", hw_alarm_count);
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}