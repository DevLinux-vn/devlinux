/*
 * File: main.c
 * Target: ESP32-S3 DevKitC-1 (Bare-Metal Register Access & Polling Only)
 *
 * Display Configuration: COMMON ANODE 7-Segment Display
 *   - Common pin is connected to 3V3.
 *   - Individual segments (a-g, dp) are connected to GPIOs via 220-330 Ohm resistors.
 *   - Segment polarity is ACTIVE-LOW (driving GPIO LOW turns segment ON; HIGH turns it OFF).
 *   - Controlled via one digit table (SEG_BITS) and named polarity inversion (SEG_POLARITY_INVERT).
 *
 * Button Configuration:
 *   - Connected between GPIO14 and GND.
 *   - Internal pull-up enabled only (internal pull-down disabled); pressed reads LOW (0).
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* =========================================================================
 * Base Addresses & Register Offsets (ESP32-S3 Technical Reference Manual)
 * ========================================================================= */
#define DR_REG_GPIO_BASE             (0x60004000UL)
#define DR_REG_IO_MUX_BASE           (0x60009000UL)

#define GPIO_OUT_W1TS_OFFSET         (0x0008UL)
#define GPIO_OUT_W1TC_OFFSET         (0x000CUL)
#define GPIO_OUT1_W1TS_OFFSET        (0x0014UL)
#define GPIO_OUT1_W1TC_OFFSET        (0x0018UL)
#define GPIO_ENABLE_W1TS_OFFSET      (0x0024UL)
#define GPIO_ENABLE_W1TC_OFFSET      (0x0028UL)
#define GPIO_ENABLE1_W1TS_OFFSET     (0x0030UL)
#define GPIO_ENABLE1_W1TC_OFFSET     (0x0034UL)
#define GPIO_IN_OFFSET               (0x003CUL)
#define GPIO_FUNC_OUT_SEL_CFG_OFFSET (0x0554UL)
#define IO_MUX_GPIO_BASE_OFFSET      (0x0004UL)

/* =========================================================================
 * Register Pointers (volatile uint32_t *)
 * ========================================================================= */
#define GPIO_OUT_W1TS_REG            (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_OUT_W1TS_OFFSET))
#define GPIO_OUT_W1TC_REG            (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_OUT_W1TC_OFFSET))
#define GPIO_OUT1_W1TS_REG           (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_OUT1_W1TS_OFFSET))
#define GPIO_OUT1_W1TC_REG           (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_OUT1_W1TC_OFFSET))
#define GPIO_ENABLE_W1TS_REG         (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_ENABLE_W1TS_OFFSET))
#define GPIO_ENABLE_W1TC_REG         (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_ENABLE_W1TC_OFFSET))
#define GPIO_ENABLE1_W1TS_REG        (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_ENABLE1_W1TS_OFFSET))
#define GPIO_ENABLE1_W1TC_REG        (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_ENABLE1_W1TC_OFFSET))
#define GPIO_IN_REG                  (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_IN_OFFSET))

/* GPIO Matrix Output Signal Selection Register for Pin n */
#define GPIO_FUNC_OUT_SEL_CFG_REG(n) (*(volatile uint32_t *)(DR_REG_GPIO_BASE + GPIO_FUNC_OUT_SEL_CFG_OFFSET + ((uint32_t)(n) * 4UL)))

/* IO_MUX Pad Configuration Register for Pin n */
#define IO_MUX_GPIO_REG(n)           (*(volatile uint32_t *)(DR_REG_IO_MUX_BASE + IO_MUX_GPIO_BASE_OFFSET + ((uint32_t)(n) * 4UL)))

/* =========================================================================
 * Bit Positions, Masks & Hardware Constants
 * ========================================================================= */
#define IO_MUX_FUN_WPD_POS           (7U)
#define IO_MUX_FUN_WPU_POS           (8U)
#define IO_MUX_FUN_IE_POS            (9U)
#define IO_MUX_FUN_DRV_POS           (10U)
#define IO_MUX_MCU_SEL_POS           (12U)

#define IO_MUX_MCU_SEL_MASK          (0x7UL)
#define IO_MUX_FUN_DRV_MASK          (0x3UL)
#define IO_MUX_FUNC_GPIO             (1UL)
#define IO_MUX_DRV_DEFAULT           (1UL)

/* GPIO Matrix simple GPIO output routing index (0x100 = 256) */
#define GPIO_OUT_SEL_MASK            (0x1FFUL)
#define GPIO_SIMPLE_OUT_SEL          (0x100UL)

#define REGISTER_WIDTH_BITS          (32U)

/* =========================================================================
 * GPIO Pin Assignments & Display Polarity
 * ========================================================================= */
#define SEG_PIN_A                    (4U)
#define SEG_PIN_B                    (5U)
#define SEG_PIN_C                    (6U)
#define SEG_PIN_D                    (7U)
#define SEG_PIN_E                    (15U)
#define SEG_PIN_F                    (16U)
#define SEG_PIN_G                    (17U)
#define SEG_PIN_DP                   (18U)
#define NUM_SEG_PINS                 (8U)

#define BTN_PIN                      (14U)
#define BTN_PRESSED_LEVEL            (0U)

/*
 * Named Polarity Inversion:
 *   1U = Common Anode   (invert table bit: 1 -> LOW turns segment ON)
 *   0U = Common Cathode (non-inverted:     1 -> HIGH turns segment ON)
 */
#define SEG_POLARITY_INVERT          (1U)

#define DIGIT_BASE                   (10U)
#define MAX_DIGIT_VALUE              (9U)

static const uint8_t LED_SEG_PINS[NUM_SEG_PINS] = {
    SEG_PIN_A, SEG_PIN_B, SEG_PIN_C, SEG_PIN_D,
    SEG_PIN_E, SEG_PIN_F, SEG_PIN_G, SEG_PIN_DP
};

/*
 * Single Digit Table (Active-High canonical representation: 1 = Segment ON)
 * Bit order: [bit 7..0] = a, b, c, d, e, f, g, dp
 */
static const uint8_t SEG_BITS[DIGIT_BASE] = {
    0b11111100U, /* '0' */
    0b01100000U, /* '1' */
    0b11011010U, /* '2' */
    0b11110010U, /* '3' */
    0b01100110U, /* '4' */
    0b10110110U, /* '5' */
    0b10111110U, /* '6' */
    0b11100000U, /* '7' */
    0b11111110U, /* '8' */
    0b11110110U  /* '9' */
};

/* =========================================================================
 * Timing Thresholds (in milliseconds)
 * ========================================================================= */
#define POLL_TICK_MS                 (10U)
#define DEBOUNCE_DELAY_MS            (25U)
#define DOUBLE_CLICK_WINDOW_MS       (350U)
#define LONG_PRESS_DELAY_MS          (800U)
#define REPEAT_PERIOD_MS             (500U)
#define LOOP_ITERATIONS_PER_MS       (1000U)

/* =========================================================================
 * Helper Functions (No gpio_*() API naming)
 * ========================================================================= */

static void delay_ms(uint32_t ms) {
    while (ms--) {
        for (volatile uint32_t i = 0U; i < LOOP_ITERATIONS_PER_MS; i++) {
            /* Busy-wait loop */
        }
    }
}

static void pin_write_high(uint32_t pin) {
    if (pin < REGISTER_WIDTH_BITS) {
        GPIO_OUT_W1TS_REG = (1UL << pin);
    } else {
        GPIO_OUT1_W1TS_REG = (1UL << (pin - REGISTER_WIDTH_BITS));
    }
}

static void pin_write_low(uint32_t pin) {
    if (pin < REGISTER_WIDTH_BITS) {
        GPIO_OUT_W1TC_REG = (1UL << pin);
    } else {
        GPIO_OUT1_W1TC_REG = (1UL << (pin - REGISTER_WIDTH_BITS));
    }
}

static void pin_enable_output(uint32_t pin) {
    if (pin < REGISTER_WIDTH_BITS) {
        GPIO_ENABLE_W1TS_REG = (1UL << pin);
    } else {
        GPIO_ENABLE1_W1TS_REG = (1UL << (pin - REGISTER_WIDTH_BITS));
    }
}

static void pin_disable_output(uint32_t pin) {
    if (pin < REGISTER_WIDTH_BITS) {
        GPIO_ENABLE_W1TC_REG = (1UL << pin);
    } else {
        GPIO_ENABLE1_W1TC_REG = (1UL << (pin - REGISTER_WIDTH_BITS));
    }
}

static uint32_t btn_pin_read(uint32_t pin) {
    return ((GPIO_IN_REG & (1UL << pin)) != 0U) ? 1U : 0U;
}

/* Initialize 7-Segment Display GPIOs */
static void seg_display_init(void) {
    for (uint32_t i = 0U; i < NUM_SEG_PINS; ++i) {
        uint32_t pin = LED_SEG_PINS[i];

        /* 1. Configure IO_MUX function as GPIO (Function 1) */
        IO_MUX_GPIO_REG(pin) &= ~(IO_MUX_MCU_SEL_MASK << IO_MUX_MCU_SEL_POS);
        IO_MUX_GPIO_REG(pin) |= (IO_MUX_FUNC_GPIO << IO_MUX_MCU_SEL_POS);

        /* 2. Set pad drive strength and disable input buffer */
        IO_MUX_GPIO_REG(pin) &= ~(IO_MUX_FUN_DRV_MASK << IO_MUX_FUN_DRV_POS);
        IO_MUX_GPIO_REG(pin) |= (IO_MUX_DRV_DEFAULT << IO_MUX_FUN_DRV_POS);
        IO_MUX_GPIO_REG(pin) &= ~(1UL << IO_MUX_FUN_IE_POS);

        /* 3. Route simple GPIO output (0x100) through the GPIO Matrix */
        GPIO_FUNC_OUT_SEL_CFG_REG(pin) =
            (GPIO_FUNC_OUT_SEL_CFG_REG(pin) & ~GPIO_OUT_SEL_MASK) | GPIO_SIMPLE_OUT_SEL;

        /* 4. Set initial output state to OFF (respecting polarity) and enable output driver */
        if (SEG_POLARITY_INVERT != 0U) {
            pin_write_high(pin); /* Common Anode OFF = HIGH */
        } else {
            pin_write_low(pin);  /* Common Cathode OFF = LOW */
        }
        pin_enable_output(pin);
    }
}

/* Write a single digit (0-9) to the 7-segment display */
static void number_to_display(uint32_t digit) {
    uint32_t index = digit % DIGIT_BASE;
    uint8_t digit_mask = SEG_BITS[index];

    for (uint32_t seg_idx = 0U; seg_idx < NUM_SEG_PINS; ++seg_idx) {
        uint32_t bit_pos = (NUM_SEG_PINS - 1U) - seg_idx;
        uint32_t seg_on = ((uint32_t)digit_mask >> bit_pos) & 1U;

        /* Apply named polarity inversion (Common Anode: 1 ^ 1 = 0 -> LOW) */
        uint32_t pin_level = seg_on ^ SEG_POLARITY_INVERT;

        if (pin_level != 0U) {
            pin_write_high(LED_SEG_PINS[seg_idx]);
        } else {
            pin_write_low(LED_SEG_PINS[seg_idx]);
        }
    }
}

/* Initialize Button GPIO (GPIO14: Input, Internal Pull-Up Only) */
static void btn_input_init(void) {
    /* 1. Select GPIO function in IO_MUX */
    IO_MUX_GPIO_REG(BTN_PIN) &= ~(IO_MUX_MCU_SEL_MASK << IO_MUX_MCU_SEL_POS);
    IO_MUX_GPIO_REG(BTN_PIN) |= (IO_MUX_FUNC_GPIO << IO_MUX_MCU_SEL_POS);

    /* 2. Enable internal pull-up ONLY, disable pull-down, enable input buffer */
    IO_MUX_GPIO_REG(BTN_PIN) |= (1UL << IO_MUX_FUN_WPU_POS);
    IO_MUX_GPIO_REG(BTN_PIN) &= ~(1UL << IO_MUX_FUN_WPD_POS);
    IO_MUX_GPIO_REG(BTN_PIN) |= (1UL << IO_MUX_FUN_IE_POS);

    /* 3. Disable output driver for input pin */
    pin_disable_output(BTN_PIN);
}

/* Helper to wait until button is released and debounce the release edge */
static void wait_for_clean_release(void) {
    do {
        while (btn_pin_read(BTN_PIN) == BTN_PRESSED_LEVEL) {
            delay_ms(POLL_TICK_MS);
        }
        delay_ms(DEBOUNCE_DELAY_MS);
    } while (btn_pin_read(BTN_PIN) == BTN_PRESSED_LEVEL);
}

/* =========================================================================
 * Main Entry Point
 * ========================================================================= */
void app_main(void) {
    printf("ESP32-S3 Bare-Metal 7-Segment & Button Polling Initialized.\n");

    seg_display_init();
    btn_input_init();

    uint32_t count = 0U;
    number_to_display(count);

    while (1) {
        /* Poll for initial button press (Active LOW) */
        if (btn_pin_read(BTN_PIN) == BTN_PRESSED_LEVEL) {
            /* Debounce press edge */
            delay_ms(DEBOUNCE_DELAY_MS);

            if (btn_pin_read(BTN_PIN) == BTN_PRESSED_LEVEL) {
                uint32_t elapsed_ms = 0U;
                bool long_press_triggered = false;

                /* Measure hold duration up to LONG_PRESS_DELAY_MS (800ms) */
                while (btn_pin_read(BTN_PIN) == BTN_PRESSED_LEVEL) {
                    if (elapsed_ms >= LONG_PRESS_DELAY_MS) {
                        long_press_triggered = true;
                        break;
                    }
                    delay_ms(POLL_TICK_MS);
                    elapsed_ms += POLL_TICK_MS;
                }

                if (long_press_triggered) {
                    /* Long press: +1 immediately on entry, then repeat every 500ms while held */
                    count = (count + 1U) % DIGIT_BASE;
                    printf("Long press entry! Count = %lu\n", (unsigned long)count);
                    number_to_display(count);

                    uint32_t repeat_elapsed = 0U;
                    while (btn_pin_read(BTN_PIN) == BTN_PRESSED_LEVEL) {
                        if (repeat_elapsed >= REPEAT_PERIOD_MS) {
                            count = (count + 1U) % DIGIT_BASE;
                            printf("Long press repeat! Count = %lu\n", (unsigned long)count);
                            number_to_display(count);
                            repeat_elapsed = 0U;
                        }
                        delay_ms(POLL_TICK_MS);
                        repeat_elapsed += POLL_TICK_MS;
                    }

                    /* Debounce release and do NOT count as a click */
                    wait_for_clean_release();
                } else {
                    /* Short press released before 800ms: debounce the release edge */
                    wait_for_clean_release();

                    uint32_t window_elapsed = 0U;
                    bool double_clicked = false;

                    /* Defer single click until DOUBLE_CLICK_WINDOW_MS (350ms) expires */
                    while (window_elapsed < DOUBLE_CLICK_WINDOW_MS) {
                        if (btn_pin_read(BTN_PIN) == BTN_PRESSED_LEVEL) {
                            /* Debounce second press edge */
                            delay_ms(DEBOUNCE_DELAY_MS);
                            if (btn_pin_read(BTN_PIN) == BTN_PRESSED_LEVEL) {
                                double_clicked = true;
                                /* Wait for second release and debounce it */
                                wait_for_clean_release();
                                break;
                            }
                        }
                        delay_ms(POLL_TICK_MS);
                        window_elapsed += POLL_TICK_MS;
                    }

                    if (double_clicked) {
                        /* Double-click produces only -1 (wraps 0 -> 9) */
                        count = (count == 0U) ? MAX_DIGIT_VALUE : (count - 1U);
                        printf("Double-click! Count = %lu\n", (unsigned long)count);
                    } else {
                        /* Deferred single-click produces +1 (wraps 9 -> 0) */
                        count = (count + 1U) % DIGIT_BASE;
                        printf("Single-click! Count = %lu\n", (unsigned long)count);
                    }

                    number_to_display(count);
                }
            }
        }
        delay_ms(POLL_TICK_MS);
    }
}
