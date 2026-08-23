/* 
 * Display Type: COMMON CATHODE
 */
#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "driver/gpio.h"

static const char *TAG = "APP";

/* --- HARDWARE REGISTER DEFINITIONS (ESP32-S3) --- */
#define GPIO_BASE                   0x60004000
#define GPIO_OUT_W1TS_REG           ((volatile uint32_t *)(GPIO_BASE + 0x0008))
#define GPIO_OUT_W1TC_REG           ((volatile uint32_t *)(GPIO_BASE + 0x000C))
#define GPIO_ENABLE_W1TS_REG        ((volatile uint32_t *)(GPIO_BASE + 0x0024))
#define IO_MUX_BASE                 0x60009000
#define IO_MUX_REG(pin)             ((volatile uint32_t *)(IO_MUX_BASE + ((pin) * 4)))
#define IO_MUX_MCU_SEL_GPIO         (1 << 12)

/* --- PINS & TIMING --- */
#define BTN_PIN                     GPIO_NUM_14
#define SEG_A_PIN                   4U
#define SEG_B_PIN                   5U
#define SEG_C_PIN                   6U
#define SEG_D_PIN                   7U
#define SEG_E_PIN                   15U
#define SEG_F_PIN                   16U
#define SEG_G_PIN                   17U

#define ALL_SEG_MASK ((1U<<SEG_A_PIN)|(1U<<SEG_B_PIN)|(1U<<SEG_C_PIN)|(1U<<SEG_D_PIN)|                       (1U<<SEG_E_PIN)|(1U<<SEG_F_PIN)|(1U<<SEG_G_PIN))

#define DEBOUNCE_MS                 25U
#define DOUBLE_CLICK_MS             350U 
#define LONG_PRESS_MS               800U 
#define REPEAT_PERIOD_MS            500U 

/* --- DATA STRUCTURES --- */
typedef struct {
    int64_t timestamp_us;
    bool    is_press; 
} btn_event_t;

static QueueHandle_t btn_queue;

/* --- SEGMENT MAPPING --- */
static const uint8_t SEGMENT_MAP[10] = {
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U, /* 0 1 2 3 4 */
    0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU  /* 5 6 7 8 9 */
};

static uint32_t get_gpio_mask_for_digit(uint8_t digit) {
    uint8_t bits = SEGMENT_MAP[digit];
    uint32_t mask = 0;
    if (bits & (1 << 0)) mask |= (1U << SEG_A_PIN);
    if (bits & (1 << 1)) mask |= (1U << SEG_B_PIN);
    if (bits & (1 << 2)) mask |= (1U << SEG_C_PIN);
    if (bits & (1 << 3)) mask |= (1U << SEG_D_PIN);
    if (bits & (1 << 4)) mask |= (1U << SEG_E_PIN);
    if (bits & (1 << 5)) mask |= (1U << SEG_F_PIN);
    if (bits & (1 << 6)) mask |= (1U << SEG_G_PIN);
    return mask;
}

static void update_display(int counter) {
    *GPIO_OUT_W1TC_REG = ALL_SEG_MASK; 
    *GPIO_OUT_W1TS_REG = get_gpio_mask_for_digit(counter);
}

/* --- HARDWARE INIT --- */
static void display_init(void) {
    uint32_t seg_pins[] = {SEG_A_PIN, SEG_B_PIN, SEG_C_PIN, SEG_D_PIN, SEG_E_PIN, SEG_F_PIN, SEG_G_PIN};
    for (int i = 0; i < 7; i++) {
        uint32_t pin = seg_pins[i];
        *IO_MUX_REG(pin) = IO_MUX_MCU_SEL_GPIO;
        *GPIO_ENABLE_W1TS_REG = (1U << pin);
    }
}

/* --- ISR --- */
static void IRAM_ATTR button_isr(void* arg) {
    btn_event_t ev;
    ev.timestamp_us = esp_timer_get_time();
    ev.is_press = (gpio_get_level(BTN_PIN) == 0); /* Pull-up: 0 is pressed */
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(btn_queue, &ev, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

/* --- MAIN APPLICATION --- */
void app_main(void) {
    display_init();
    
    int counter = 0;
    update_display(counter);
    
    btn_queue = xQueueCreate(20, sizeof(btn_event_t));

    /* Driver API for Interrupt */
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BTN_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_PIN, button_isr, NULL);

    bool is_physically_pressed = false;
    bool long_press_active = false;
    bool pending_click = false;
    bool ignore_next_release = false;
    
    int64_t last_edge_time_us = 0;
    int64_t last_repeat_time_us = 0;
    int64_t click_release_time_us = 0;

    while (1) {
        TickType_t wait_ticks = portMAX_DELAY;
        int64_t now_us = esp_timer_get_time();
        
        /* Calculate Queue Timeout natively for gestures */
        if (is_physically_pressed) {
            if (!long_press_active) {
                int64_t remaining = (LONG_PRESS_MS * 1000LL) - (now_us - last_edge_time_us);
                wait_ticks = (remaining > 0) ? pdMS_TO_TICKS(remaining / 1000) : 0;
            } else {
                int64_t remaining = (REPEAT_PERIOD_MS * 1000LL) - (now_us - last_repeat_time_us);
                wait_ticks = (remaining > 0) ? pdMS_TO_TICKS(remaining / 1000) : 0;
            }
        } else if (pending_click) {
            int64_t remaining = (DOUBLE_CLICK_MS * 1000LL) - (now_us - click_release_time_us);
            wait_ticks = (remaining > 0) ? pdMS_TO_TICKS(remaining / 1000) : 0;
        }

        btn_event_t ev;
        if (xQueueReceive(btn_queue, &ev, wait_ticks) == pdTRUE) {
            /* 1. Hardware Event Arrived from ISR */
            if ((ev.timestamp_us - last_edge_time_us) < (DEBOUNCE_MS * 1000LL)) {
                continue; /* Bounce edge -> Ignore */
            }
            if (ev.is_press == is_physically_pressed) {
                continue; /* No logical change -> Ignore */
            }
            
            is_physically_pressed = ev.is_press;
            last_edge_time_us = ev.timestamp_us;
            
            if (is_physically_pressed) {
                if (pending_click) {
                    /* Second click arrived! */
                    pending_click = false;
                    ignore_next_release = true; /* Prevent this press's release from acting as a 3rd click */
                    counter = (counter + 9) % 10; /* Wrap -1 */
                    update_display(counter);
                    ESP_LOGI(TAG, "BTN: DOUBLE     -> %d", counter);
                } else {
                    long_press_active = false;
                }
            } else {
                if (ignore_next_release) {
                    ignore_next_release = false;
                } else if (long_press_active) {
                    long_press_active = false;
                    ESP_LOGI(TAG, "BTN: LONG end");
                } else {
                    /* First click released, start wait for double-click */
                    pending_click = true;
                    click_release_time_us = ev.timestamp_us;
                }
            }
        } else {
            /* 2. TIMEOUT Triggered (No physical edges, pure logical time passing) */
            now_us = esp_timer_get_time();
            if (is_physically_pressed) {
                if (!long_press_active) {
                    long_press_active = true;
                    pending_click = false; /* Invalidate */
                    counter = (counter + 1) % 10;
                    update_display(counter);
                    last_repeat_time_us = now_us;
                    ESP_LOGI(TAG, "BTN: LONG start -> %d", counter);
                } else {
                    counter = (counter + 1) % 10;
                    update_display(counter);
                    last_repeat_time_us = now_us;
                    ESP_LOGI(TAG, "BTN: LONG rep   -> %d", counter);
                }
            } else if (pending_click) {
                pending_click = false;
                counter = (counter + 1) % 10;
                update_display(counter);
                ESP_LOGI(TAG, "BTN: CLICK      -> %d", counter);
            }
        }
    }
}
