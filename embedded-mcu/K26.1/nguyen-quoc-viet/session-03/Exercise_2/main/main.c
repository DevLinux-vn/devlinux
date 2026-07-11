/*
 * Session 03 - Exercise 2
 * Read a push-button on GPIO4 (input, internal pull-up) and mirror its
 * state onto the LED on GPIO45 (button pressed = LED on).
 *
 * Framework: ESP-IDF (esp32-s3). NO Arduino code / NO Arduino.h.
 * Everything (input AND output) is configured/driven at the REGISTER
 * LEVEL only. Polling loop, no interrupts (interrupts are Session 04).
 *
 * GPIO4  < 32  -> bank0 registers (GPIO_ENABLE_*, GPIO_IN_REG)
 * GPIO45 >= 32 -> bank1 registers (GPIO_ENABLE1_*, GPIO_OUT1_*)
 *
 * All addresses/offsets below are verified against Espressif's official
 * ESP-IDF SoC register headers:
 *   components/soc/esp32s3/register/soc/{reg_base,gpio_reg,io_mux_reg}.h
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

#define GPIO_IN_REG_OFFSET               (0x3CU)
#define GPIO_IN1_REG_OFFSET              (0x40U)

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

#define GPIO_IN_REG                GPIO_REG(GPIO_IN_REG_OFFSET)
#define GPIO_IN1_REG               GPIO_REG(GPIO_IN1_REG_OFFSET)

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
 *   #define FUNC_GPIO4_GPIO4   1
 */
#define IO_MUX_FUNC_GPIO         (1U)

/* ------------------------------------------------------------------ */
/*  Application constants                                             */
/* ------------------------------------------------------------------ */
#define REGISTER_WIDTH_BITS      (32U)
#define GPIO_BANK_SPLIT_PIN      (32U)  /* pins >= this value live in bank1 */

#define LED_PIN                  (45U)
#define LED_DRIVE_STRENGTH       (2U)   /* FUN_DRV = 2 -> ~20 mA */

#define BTN_PIN                  (4U)
#define BTN_POLL_DELAY_MS        (50U)
#define BTN_PRESSED_LEVEL        (0U)   /* pull-up: pressed pulls the line LOW */

static const char *TAG = "Exercise2_RegisterPollButtonLED";

/* ------------------------------------------------------------------ */
/*  IO_MUX configuration                                               */
/* ------------------------------------------------------------------ */

static void io_mux_config_gpio_output(uint32_t pin)
{
    volatile uint32_t *reg = &IO_MUX_GPIO_REG(pin);
    uint32_t val = *reg;

    val &= ~(IO_MUX_MCU_SEL_MASK << IO_MUX_MCU_SEL_SHIFT);
    val |= (IO_MUX_FUNC_GPIO & IO_MUX_MCU_SEL_MASK) << IO_MUX_MCU_SEL_SHIFT;

    val &= ~(1U << IO_MUX_FUN_IE_BIT);   /* output pin: no need for digital input */
    val &= ~(1U << IO_MUX_FUN_WPU_BIT);
    val &= ~(1U << IO_MUX_FUN_WPD_BIT);

    val &= ~(IO_MUX_FUN_DRV_MASK << IO_MUX_FUN_DRV_SHIFT);
    val |= (LED_DRIVE_STRENGTH & IO_MUX_FUN_DRV_MASK) << IO_MUX_FUN_DRV_SHIFT;

    *reg = val;
}

/*
 * Configure `pin` as a digital input with the internal pull-up enabled
 * and pull-down disabled, purely via IO_MUX (FUN_IE / FUN_WPU / FUN_WPD).
 */
static void io_mux_config_gpio_input_pullup(uint32_t pin)
{
    volatile uint32_t *reg = &IO_MUX_GPIO_REG(pin);
    uint32_t val = *reg;

    val &= ~(IO_MUX_MCU_SEL_MASK << IO_MUX_MCU_SEL_SHIFT);
    val |= (IO_MUX_FUNC_GPIO & IO_MUX_MCU_SEL_MASK) << IO_MUX_MCU_SEL_SHIFT;

    val |= (1U << IO_MUX_FUN_IE_BIT);    /* enable digital input path */
    val |= (1U << IO_MUX_FUN_WPU_BIT);   /* enable internal pull-up  */
    val &= ~(1U << IO_MUX_FUN_WPD_BIT);  /* disable pull-down         */

    *reg = val;
}

/* ------------------------------------------------------------------ */
/*  GPIO_ENABLE helpers (output-enable bit, both banks)                */
/* ------------------------------------------------------------------ */

static void gpio_register_enable_output(uint32_t pin)
{
    if (pin < GPIO_BANK_SPLIT_PIN) {
        GPIO_ENABLE_W1TS_REG = (1U << pin);
    } else {
        GPIO_ENABLE1_W1TS_REG = (1U << (pin - GPIO_BANK_SPLIT_PIN));
    }
}

static void gpio_register_disable_output(uint32_t pin)
{
    if (pin < GPIO_BANK_SPLIT_PIN) {
        GPIO_ENABLE_W1TC_REG = (1U << pin);
    } else {
        GPIO_ENABLE1_W1TC_REG = (1U << (pin - GPIO_BANK_SPLIT_PIN));
    }
}

/* ------------------------------------------------------------------ */
/*  LED helper functions (same as Session 03 / Exercise 1, reused)     */
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

/*
 * Read the raw logic level of `pin` from GPIO_IN_REG / GPIO_IN1_REG,
 * choosing the correct bank based on the pin number.
 */
static uint32_t gpio_register_read(uint32_t pin)
{
    uint32_t level;

    if (pin < GPIO_BANK_SPLIT_PIN) {
        level = (GPIO_IN_REG >> pin) & 0x1U;
    } else {
        level = (GPIO_IN1_REG >> (pin - GPIO_BANK_SPLIT_PIN)) & 0x1U;
    }

    return level;
}

/* ------------------------------------------------------------------ */
/*  Initialisation                                                     */
/* ------------------------------------------------------------------ */

static void gpio_init_led(void)
{
    io_mux_config_gpio_output(LED_PIN);
    gpio_register_enable_output(LED_PIN);
    gpio_register_set_low(LED_PIN); /* start off */

    ESP_LOGI(TAG, "LED on GPIO%u configured (output)", LED_PIN);
}

static void gpio_init_button(void)
{
    io_mux_config_gpio_input_pullup(BTN_PIN);
    gpio_register_disable_output(BTN_PIN); /* pure input, no output driver */

    ESP_LOGI(TAG, "Button on GPIO%u configured (input, pull-up)", BTN_PIN);
}

/* ------------------------------------------------------------------ */
/*  app_main                                                           */
/* ------------------------------------------------------------------ */

void app_main(void)
{
    gpio_init_led();
    gpio_init_button();

    while (1) {
        uint32_t btn_level = gpio_register_read(BTN_PIN);

        if (btn_level == BTN_PRESSED_LEVEL) {
            gpio_register_set_high(LED_PIN);
            ESP_LOGI(TAG, "Button pressed -> LED ON");
        } else {
            gpio_register_set_low(LED_PIN);
            ESP_LOGI(TAG, "Button released -> LED OFF");
        }

        vTaskDelay(pdMS_TO_TICKS(BTN_POLL_DELAY_MS));
    }
}
