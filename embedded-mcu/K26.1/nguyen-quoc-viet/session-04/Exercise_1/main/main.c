/*
 * Session 04 - Exercise_1
 * Interrupt-driven button toggle for LED (GPIO45) triggered by
 * falling-edge interrupt on button (GPIO4).
 *
 * - Button detection: Driver API (gpio_config + gpio_isr_handler_add)
 * - LED drive: register-level access (bank1, from Session 03 Exercise_1)
 *
 * Register anchors (verified against ESP-IDF ESP32-S3 SoC headers):
 *   DR_REG_GPIO_BASE   = 0x60004000
 *   DR_REG_IO_MUX_BASE = 0x60009000
 *   IO_MUX offset      = 0x04 + pin * 4
 *   MCU_SEL = 1 -> pin routed to GPIO function
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_log.h"

/* -------------------------------------------------------------------- */
/*  Register-level LED control — GPIO45 (bank1: pins 32..56)            */
/* -------------------------------------------------------------------- */

#define DR_REG_GPIO_BASE     0x60004000
#define DR_REG_IO_MUX_BASE   0x60009000

/* GPIO bank1 (covers pins 32-56) */
#define GPIO_OUT1_REG          (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x0010))
#define GPIO_OUT1_W1TS_REG     (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x0014))
#define GPIO_OUT1_W1TC_REG     (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x0018))
#define GPIO_ENABLE1_REG       (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x002C))
#define GPIO_ENABLE1_W1TS_REG  (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x0030))
#define GPIO_ENABLE1_W1TC_REG  (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x0034))

#define LED_PIN         45
#define LED_BANK1_BIT   (LED_PIN - 32)     /* bit 13 within bank1 */

/* IO_MUX register for pin 45: offset = 0x04 + pin*4 */
#define IO_MUX_GPIO45_REG   (*(volatile uint32_t *)(DR_REG_IO_MUX_BASE + 0x04 + (LED_PIN * 4)))
#define IO_MUX_MCU_SEL_S    12
#define IO_MUX_MCU_SEL_GPIO 1
#define IO_MUX_FUN_DRV_S    10

static void gpio_register_led_init(void)
{
    /* Route pin 45 to GPIO matrix function (MCU_SEL = 1), max drive strength */
    uint32_t muxval = IO_MUX_GPIO45_REG;
    muxval &= ~(0x7u << IO_MUX_MCU_SEL_S);
    muxval |= ((uint32_t)IO_MUX_MCU_SEL_GPIO << IO_MUX_MCU_SEL_S);
    muxval |= (0x3u << IO_MUX_FUN_DRV_S);
    IO_MUX_GPIO45_REG = muxval;

    /* Enable pin 45 as output (bank1, bit13) */
    GPIO_ENABLE1_W1TS_REG = (1UL << LED_BANK1_BIT);
}

static inline void gpio_register_set_high(void)
{
    GPIO_OUT1_W1TS_REG = (1UL << LED_BANK1_BIT);
}

static inline void gpio_register_set_low(void)
{
    GPIO_OUT1_W1TC_REG = (1UL << LED_BANK1_BIT);
}

/* -------------------------------------------------------------------- */
/*  Interrupt-driven button — GPIO4, Driver API                        */
/* -------------------------------------------------------------------- */

#define BTN_PIN 4

static const char *TAG = "session04_ex1";

static volatile bool btn_pressed = false;  /* set by ISR, cleared in main loop */
static bool led_state = false;             /* current LED state (app_main only) */

/*
 * ISR: absolute minimum work — set a flag and return.
 * No logging, no delays, no LED toggling logic here.
 */
static void IRAM_ATTR button_isr(void *arg)
{
    btn_pressed = true;
}

static void button_interrupt_init(void)
{
    gpio_config_t btn_cfg = {
        .pin_bit_mask = (1ULL << BTN_PIN),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&btn_cfg);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_PIN, button_isr, NULL);
}

/* -------------------------------------------------------------------- */
/*  app_main                                                            */
/* -------------------------------------------------------------------- */

void app_main(void)
{
    gpio_register_led_init();
    gpio_register_set_low();          /* LED starts OFF */

    button_interrupt_init();

    ESP_LOGI(TAG, "Ready. Press button on GPIO%d to toggle LED on GPIO%d.",
             BTN_PIN, LED_PIN);

    while (1) {
        if (btn_pressed) {
            btn_pressed = false;      /* clear the flag first */

            led_state = !led_state;   /* all real logic lives here, not in ISR */
            if (led_state) {
                gpio_register_set_high();
            } else {
                gpio_register_set_low();
            }

            ESP_LOGI(TAG, "Button pressed -> LED %s", led_state ? "ON" : "OFF");
        }

        vTaskDelay(pdMS_TO_TICKS(50)); /* idle housekeeping only, not detection */
    }
}
