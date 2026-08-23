/* 
 * Display Type: COMMON CATHODE 
 * (As chosen per assignment instructions - inverted logic not needed for cathode)
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "COUNTER";

/* ========================================================================= *
 * 1. HARDWARE REGISTER DEFINITIONS (ESP32-S3)
 * ========================================================================= */
#define GPIO_BASE                   0x60004000
#define GPIO_OUT_W1TS_REG           ((volatile uint32_t *)(GPIO_BASE + 0x0008))
#define GPIO_OUT_W1TC_REG           ((volatile uint32_t *)(GPIO_BASE + 0x000C))
#define GPIO_ENABLE_W1TS_REG        ((volatile uint32_t *)(GPIO_BASE + 0x0024))
#define GPIO_IN_REG                 ((volatile uint32_t *)(GPIO_BASE + 0x003C))

#define IO_MUX_BASE                 0x60009000
#define IO_MUX_REG(pin)             ((volatile uint32_t *)(IO_MUX_BASE + ((pin) * 4)))

/* IO MUX Bit Masks */
#define IO_MUX_MCU_SEL_GPIO         (1 << 12) /* Set function to plain GPIO */
#define IO_MUX_MCU_IE               (1 << 9)  /* Input Enable */
#define IO_MUX_MCU_WPU              (1 << 7)  /* Internal Pull-up Enable */

/* ========================================================================= *
 * 2. PIN ASSIGNMENTS & MASKS
 * ========================================================================= */
#define BTN_PIN                     (14U)
#define BTN_MASK                    (1U << BTN_PIN)

/* Segments: a=4, b=5, c=6, d=7, e=15, f=16, g=17 */
#define SEG_A_PIN (4U)
#define SEG_B_PIN (5U)
#define SEG_C_PIN (6U)
#define SEG_D_PIN (7U)
#define SEG_E_PIN (15U)
#define SEG_F_PIN (16U)
#define SEG_G_PIN (17U)

/* Mask for all segments to clear the display easily in one write */
#define ALL_SEG_MASK ((1U<<SEG_A_PIN)|(1U<<SEG_B_PIN)|(1U<<SEG_C_PIN)|(1U<<SEG_D_PIN)| \
                      (1U<<SEG_E_PIN)|(1U<<SEG_F_PIN)|(1U<<SEG_G_PIN))

/* ========================================================================= *
 * 3. TIMING THRESHOLDS (in milliseconds)
 * ========================================================================= */
#define DEBOUNCE_MS                 (25U)
#define DOUBLE_CLICK_MS             (350U) 
#define LONG_PRESS_MS               (800U) 
#define REPEAT_PERIOD_MS            (500U) 
#define POLLING_RATE_MS             (5U)   /* Short fixed period for polling */

/* ========================================================================= *
 * 4. SEGMENT MAPPING
 * ========================================================================= */
static const uint8_t SEGMENT_MAP[10] = {
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U, /* 0 1 2 3 4 */
    0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU  /* 5 6 7 8 9 */
};

/* Convert a 7-bit segment map into a 32-bit GPIO mask */
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

/* ========================================================================= *
 * 5. HARDWARE INITIALIZATION
 * ========================================================================= */
static void hw_init(void) {
    /* Initialize Output Pins (Segments) */
    uint32_t seg_pins[] = {SEG_A_PIN, SEG_B_PIN, SEG_C_PIN, SEG_D_PIN, SEG_E_PIN, SEG_F_PIN, SEG_G_PIN};
    for (int i = 0; i < 7; i++) {
        uint32_t pin = seg_pins[i];
        *IO_MUX_REG(pin) = IO_MUX_MCU_SEL_GPIO; /* Route to plain GPIO */
        *GPIO_ENABLE_W1TS_REG = (1U << pin);    /* Enable Output Driver */
    }

    /* Initialize Input Pin (Button on GPIO 14) */
    *IO_MUX_REG(BTN_PIN) = IO_MUX_MCU_SEL_GPIO | IO_MUX_MCU_IE | IO_MUX_MCU_WPU;
}

static void update_display(int counter) {
    /* Write to W1TC and W1TS allows changing all bits at once seamlessly */
    *GPIO_OUT_W1TC_REG = ALL_SEG_MASK; 
    *GPIO_OUT_W1TS_REG = get_gpio_mask_for_digit(counter);
}

/* ========================================================================= *
 * 6. MAIN APPLICATION & STATE MACHINE
 * ========================================================================= */
void app_main(void) {
    hw_init();
    
    int counter = 0;
    update_display(counter);
    ESP_LOGI(TAG, "System Reset. Displaying 0");

    /* Debounce state variables */
    int debounced_state = 1; /* Unpressed (Pull-up) */
    int last_raw_state = 1;
    uint32_t last_debounce_time = 0;

    /* Gesture decoding variables */
    bool is_pressed = false;
    bool long_press_active = false;
    bool pending_click = false;
    
    uint32_t press_time = 0;
    uint32_t release_time = 0;
    uint32_t last_repeat_time = 0;

    while (1) {
        uint32_t now = (uint32_t)(esp_timer_get_time() / 1000ULL); // Time in ms
        
        /* --- 1. DEBOUNCING LOGIC --- */
        int raw_state = (*GPIO_IN_REG & BTN_MASK) ? 1 : 0;
        
        if (raw_state != last_raw_state) {
            last_debounce_time = now;
        }
        
        if ((now - last_debounce_time) > DEBOUNCE_MS) {
            if (raw_state != debounced_state) {
                debounced_state = raw_state;
                
                if (debounced_state == 0) {
                    /* EVENT: BUTTON PRESSED (1 -> 0) */
                    is_pressed = true;
                    press_time = now;
                } else {
                    /* EVENT: BUTTON RELEASED (0 -> 1) */
                    is_pressed = false;
                    release_time = now;
                    
                    if (long_press_active) {
                        /* Releasing a long press doesn't add an extra count */
                        long_press_active = false; 
                    } else {
                        /* It was a short press */
                        if (pending_click) {
                            /* Second click arrived! */
                            pending_click = false;
                            counter = (counter - 1 + 10) % 10; // Double click: -1
                            update_display(counter);
                            ESP_LOGI(TAG, "Double Click! Counter: %d", counter);
                        } else {
                            /* First click, wait to see if a second one comes */
                            pending_click = true;
                        }
                    }
                }
            }
        }
        last_raw_state = raw_state;

        /* --- 2. GESTURE EVALUATION LOGIC --- */
        if (is_pressed) {
            /* Check for Long Press Start */
            if (!long_press_active && ((now - press_time) >= LONG_PRESS_MS)) {
                long_press_active = true;
                pending_click = false; /* Invalidate any accidental pending click */
                
                counter = (counter + 1) % 10;
                update_display(counter);
                last_repeat_time = now;
                ESP_LOGI(TAG, "Long Press Start! Counter: %d", counter);
            }
            /* Check for Long Press Auto-Repeat */
            else if (long_press_active && ((now - last_repeat_time) >= REPEAT_PERIOD_MS)) {
                counter = (counter + 1) % 10;
                update_display(counter);
                last_repeat_time = now;
                ESP_LOGI(TAG, "Long Press Repeat! Counter: %d", counter);
            }
        } else {
            /* If button is released, evaluate if we need to commit the pending single click */
            if (pending_click && ((now - release_time) >= DOUBLE_CLICK_MS)) {
                pending_click = false;
                counter = (counter + 1) % 10; // Single click: +1
                update_display(counter);
                ESP_LOGI(TAG, "Single Click! Counter: %d", counter);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(POLLING_RATE_MS));
    }
}