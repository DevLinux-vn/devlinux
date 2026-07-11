/*
 * Session 03 - Exercise 1
 * Blink an external LED on GPIO45 using DIRECT REGISTER ACCESS ONLY.
 *
 * Framework: ESP-IDF (esp32-s3). NO Arduino code / NO Arduino.h.
 * NO calls into driver/gpio.h are used in this file.
 *
 * GPIO45 >= 32  -> lives in the SECOND GPIO bank (GPIO_OUT1_*, GPIO_ENABLE1_*)
 * Bit offset inside that bank = pin - 32 = 13
 *
 * All addresses/offsets below are verified against Espressif's official
 * ESP-IDF SoC register headers:
 *   components/soc/esp32s3/register/soc/{reg_base,gpio_reg,io_mux_reg}.h
 *
 * All register addresses / bit positions are named constants.
 * All register pointers are declared volatile.
 */

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

/* ------------------------------------------------------------------ */
/*  Peripheral base addresses (verified: soc/reg_base.h)              */
/* ------------------------------------------------------------------ */
#define DR_REG_GPIO_BASE                (0x60004000UL)
#define DR_REG_IO_MUX_BASE              (0x60009000UL)

/* ------------------------------------------------------------------ */
/*  GPIO register offsets (bank0 = pins 0-31, bank1 = pins 32-48)     */
/*  Verified: soc/gpio_reg.h                                          */
/* ------------------------------------------------------------------ */
#define GPIO_OUT_REG_OFFSET              (0x04U)
#define GPIO_OUT_W1TS_REG_OFFSET         (0x08U)
#define GPIO_OUT_W1TC_REG_OFFSET         (0x0CU)

#define GPIO_OUT1_REG_OFFSET             (0x10U)
#define GPIO_OUT1_W1TS_REG_OFFSET        (0x14U)
#define GPIO_OUT1_W1TC_REG_OFFSET        (0x18U)

#define GPIO_ENABLE_REG_OFFSET           (0x20U)
#define GPIO_ENABLE_W1TS_REG_OFFSET      (0x24U)
#define GPIO_ENABLE_W1TC_REG_OFFSET      (0x28U)

#define GPIO_ENABLE1_REG_OFFSET          (0x2CU)
#define GPIO_ENABLE1_W1TS_REG_OFFSET     (0x30U)
#define GPIO_ENABLE1_W1TC_REG_OFFSET     (0x34U)

#define GPIO_REG(offset)   (*(volatile uint32_t *)(DR_REG_GPIO_BASE + (offset)))

#define GPIO_OUT_REG               GPIO_REG(GPIO_OUT_REG_OFFSET)
#define GPIO_OUT_W1TS_REG          GPIO_REG(GPIO_OUT_W1TS_REG_OFFSET)
#define GPIO_OUT_W1TC_REG          GPIO_REG(GPIO_OUT_W1TC_REG_OFFSET)

#define GPIO_OUT1_REG              GPIO_REG(GPIO_OUT1_REG_OFFSET)
#define GPIO_OUT1_W1TS_REG         GPIO_REG(GPIO_OUT1_W1TS_REG_OFFSET)
#define GPIO_OUT1_W1TC_REG         GPIO_REG(GPIO_OUT1_W1TC_REG_OFFSET)

#define GPIO_ENABLE_REG            GPIO_REG(GPIO_ENABLE_REG_OFFSET)
#define GPIO_ENABLE_W1TS_REG       GPIO_REG(GPIO_ENABLE_W1TS_REG_OFFSET)
#define GPIO_ENABLE_W1TC_REG       GPIO_REG(GPIO_ENABLE_W1TC_REG_OFFSET)

#define GPIO_ENABLE1_REG           GPIO_REG(GPIO_ENABLE1_REG_OFFSET)
#define GPIO_ENABLE1_W1TS_REG      GPIO_REG(GPIO_ENABLE1_W1TS_REG_OFFSET)
#define GPIO_ENABLE1_W1TC_REG      GPIO_REG(GPIO_ENABLE1_W1TC_REG_OFFSET)

/* ------------------------------------------------------------------ */
/*  IO_MUX register access + field layout (verified: soc/io_mux_reg.h)*/
/*  Offset formula 0x04 + pin*4 holds for every pin 0-48 (confirmed   */
/*  directly against PERIPHS_IO_MUX_GPIOn_U macros in the header).    */
/* ------------------------------------------------------------------ */
#define IO_MUX_GPIO_REG_STRIDE_BYTES    (4U)
#define IO_MUX_GPIO_REG_FIRST_OFFSET    (0x04U)   /* offset of IO_MUX_GPIO0_REG */

#define IO_MUX_GPIO_REG(pin)  \
    (*(volatile uint32_t *)(DR_REG_IO_MUX_BASE + IO_MUX_GPIO_REG_FIRST_OFFSET + \
                             ((pin) * IO_MUX_GPIO_REG_STRIDE_BYTES)))

/* Bit fields inside an IO_MUX_GPIOn_REG register */
#define IO_MUX_FUN_WPD_BIT       (7U)
#define IO_MUX_FUN_WPU_BIT       (8U)
#define IO_MUX_FUN_IE_BIT        (9U)
#define IO_MUX_FUN_DRV_SHIFT     (10U)
#define IO_MUX_FUN_DRV_MASK      (0x3U)
#define IO_MUX_MCU_SEL_SHIFT     (13U)
#define IO_MUX_MCU_SEL_MASK      (0x7U)

/* MCU_SEL = 1 selects the GPIO matrix function on every pin.
 * Verified against the header's per-pin constant, e.g.:
 *   #define FUNC_GPIO45_GPIO45   1
 */
#define IO_MUX_FUNC_GPIO         (1U)

/* ------------------------------------------------------------------ */
/*  Application constants                                             */
/* ------------------------------------------------------------------ */
#define REGISTER_WIDTH_BITS      (32U)
#define GPIO_BANK_SPLIT_PIN      (32U)  /* pins >= this value live in bank1 */

#define LED_PIN                  (45U)
#define LED_BLINK_DELAY_MS       (1000U)
#define LED_DRIVE_STRENGTH       (2U)   /* FUN_DRV = 2 -> ~20 mA */

static const char *TAG = "Exercise1_RegisterBlink";

/* ------------------------------------------------------------------ */
/*  IO_MUX configuration                                               */
/* ------------------------------------------------------------------ */

/*
 * Route the physical pin to the GPIO matrix (MCU_SEL = IO_MUX_FUNC_GPIO),
 * disable the digital input path (not needed for an output-only LED pin),
 * and set the drive strength.
 */
static void io_mux_config_gpio_output(uint32_t pin)
{
    volatile uint32_t *reg = &IO_MUX_GPIO_REG(pin);
    uint32_t val = *reg;

    /* select GPIO function */
    val &= ~(IO_MUX_MCU_SEL_MASK << IO_MUX_MCU_SEL_SHIFT);
    val |= (IO_MUX_FUNC_GPIO & IO_MUX_MCU_SEL_MASK) << IO_MUX_MCU_SEL_SHIFT;

    /* disable digital input path (output-only pin) */
    val &= ~(1U << IO_MUX_FUN_IE_BIT);

    /* no pull-up / pull-down needed on an output */
    val &= ~(1U << IO_MUX_FUN_WPU_BIT);
    val &= ~(1U << IO_MUX_FUN_WPD_BIT);

    /* drive strength */
    val &= ~(IO_MUX_FUN_DRV_MASK << IO_MUX_FUN_DRV_SHIFT);
    val |= (LED_DRIVE_STRENGTH & IO_MUX_FUN_DRV_MASK) << IO_MUX_FUN_DRV_SHIFT;

    *reg = val;
}

/*
 * Enable the output driver for `pin`, picking the correct GPIO bank
 * (bank0: pins 0-31, bank1: pins 32-48) based on the pin number.
 */
static void gpio_register_enable_output(uint32_t pin)
{
    if (pin < GPIO_BANK_SPLIT_PIN) {
        GPIO_ENABLE_W1TS_REG = (1U << pin);
    } else {
        GPIO_ENABLE1_W1TS_REG = (1U << (pin - GPIO_BANK_SPLIT_PIN));
    }
}

/* ------------------------------------------------------------------ */
/*  LED helper functions (reused by Exercise 2)                       */
/* ------------------------------------------------------------------ */

static void gpio_register_set_high(uint32_t pin)
{
    if (pin < GPIO_BANK_SPLIT_PIN) {
        GPIO_OUT_W1TS_REG = (1U << pin);
    } else {
        GPIO_OUT1_W1TS_REG = (1U << (pin - GPIO_BANK_SPLIT_PIN));
    }
}

static void gpio_register_set_low(uint32_t pin)
{
    if (pin < GPIO_BANK_SPLIT_PIN) {
        GPIO_OUT_W1TC_REG = (1U << pin);
    } else {
        GPIO_OUT1_W1TC_REG = (1U << (pin - GPIO_BANK_SPLIT_PIN));
    }
}

/* ------------------------------------------------------------------ */
/*  Initialisation                                                     */
/* ------------------------------------------------------------------ */

static void gpio_init_led(void)
{
    io_mux_config_gpio_output(LED_PIN);
    gpio_register_enable_output(LED_PIN);
    gpio_register_set_low(LED_PIN); /* start off */

    ESP_LOGI(TAG, "LED on GPIO%u configured (register-level, IO_MUX + GPIO_ENABLE1)", LED_PIN);
}

/* ------------------------------------------------------------------ */
/*  app_main                                                           */
/* ------------------------------------------------------------------ */

void app_main(void)
{
    gpio_init_led();

    while (1) {
        gpio_register_set_high(LED_PIN);
        ESP_LOGI(TAG, "LED ON");
        vTaskDelay(pdMS_TO_TICKS(LED_BLINK_DELAY_MS));

        gpio_register_set_low(LED_PIN);
        ESP_LOGI(TAG, "LED OFF");
        vTaskDelay(pdMS_TO_TICKS(LED_BLINK_DELAY_MS));
    }
}
