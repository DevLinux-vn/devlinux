/* HEADER
 * INPUT: Button
 * OUTPUT: 7-Segment Display with Common Anode
 * METHOD: Bare Metal and Polling
 */

#include <stdbool.h>
#include <stdio.h>
// typedef unsigned int uint32_t;

#define DR_REG_GPIO_BASE (0x60004000UL)
#define DR_REG_IO_MUX_BASE (0x60009000UL)

#define GPIO_ENABLE_W1TS_REG (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x24U))
#define GPIO_ENABLE1_W1TS_REG (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x30U))
#define GPIO_ENABLE_W1TC_REG (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x28U))
#define GPIO_ENABLE1_W1TC_REG (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x34U))
#define GPIO_OUT_W1TS_REG (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x08U))
#define GPIO_OUT1_W1TS_REG (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x14U))
#define GPIO_OUT_W1TC_REG (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x0CU))
#define GPIO_OUT1_W1TC_REG (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x18U))
#define GPIO_IN_REG (*(volatile uint32_t *)(DR_REG_GPIO_BASE + 0x3CU))

#define IO_MUX_GPIO_REG(n)                                                     \
  (*(volatile uint32_t *)(DR_REG_IO_MUX_BASE + 0x4U + (n) * 4U))

// Bit positions inside IO_MUX_GPIOn_REG
#define IO_MUX_FUN_WPD_POS (7U)
#define IO_MUX_FUN_WPU_POS (8U)
#define IO_MUX_FUN_IE_POS (9U)
#define IO_MUX_FUN_DRV_POS (10U)
#define IO_MUX_MCU_SEL_POS (12U)
#define IO_MUX_FUNC_GPIO (1U)

#define REGISTER_WIDTH_BITS (32U)

// Define 7-SEGMENT-DISPLAY pins
const uint32_t LED_SEG_PINS[8] = {4U, 5U, 6U, 7U, 15U, 16U, 17U, 18U};

const uint32_t SEG_BITS[10] = {
    // abcdefgdp
    0b11111100U, // '0'
    0b01100000U, // '1'
    0b11011010U, // '2'
    0b11110010U, // '3'
    0b01100110U, // '4'
    0b10110110U, // '5'
    0b10111110U, // '6'
    0b11100000U, // '7'
    0b11111110U, // '8'
    0b11110110U  // '9'
};

// Define BUTTON pin
#define BTN_PIN (14U)

#define DEBOUNCE_DELAY_MS (25U)
#define DOUBLE_CLICK_WINDOW_MS (350U)
#define LONG_PRESS_DELAY_MS (800U)
#define REPEAT_PERIOD_MS (500U)

// Custom software delay function
void delay_ms(uint32_t ms) {
  while (ms--) {
    for (volatile uint32_t i = 0U; i < 1000U; i++) {
      // Do nothing
    }
  }
}

// 7-SEGMENT Functions
void gpio_init_led_seg(void) {
  for (int i = 0; i < 8; ++i) {
    IO_MUX_GPIO_REG(LED_SEG_PINS[i]) &= ~(7UL << IO_MUX_MCU_SEL_POS);
    IO_MUX_GPIO_REG(LED_SEG_PINS[i]) |=
        ((uint32_t)IO_MUX_FUNC_GPIO << IO_MUX_MCU_SEL_POS);

    IO_MUX_GPIO_REG(LED_SEG_PINS[i]) &= ~(3UL << IO_MUX_FUN_DRV_POS);
    IO_MUX_GPIO_REG(LED_SEG_PINS[i]) |= (1UL << IO_MUX_FUN_DRV_POS);

    IO_MUX_GPIO_REG(LED_SEG_PINS[i]) &= ~(1UL << IO_MUX_FUN_IE_POS);

    GPIO_ENABLE_W1TS_REG = (1UL << LED_SEG_PINS[i]);
    GPIO_OUT_W1TS_REG = (1UL << LED_SEG_PINS[i]);
  }
}

void gpio_register_set_high(uint32_t pin) {
  if (pin < REGISTER_WIDTH_BITS) {
    GPIO_OUT_W1TS_REG = (1UL << pin);
  } else {
    GPIO_OUT1_W1TS_REG = (1UL << (pin - REGISTER_WIDTH_BITS));
  }
}

void gpio_register_set_low(uint32_t pin) {
  if (pin < REGISTER_WIDTH_BITS) {
    GPIO_OUT_W1TC_REG = (1UL << pin);
  } else {
    GPIO_OUT1_W1TC_REG = (1UL << (pin - REGISTER_WIDTH_BITS));
  }
}

void number_to_display(uint32_t digit) {
  uint32_t index = digit % 10U;
  unsigned char digit_mask = SEG_BITS[index];

  for (int i = 7; i >= 0; i--) {
    int bit = (digit_mask >> i) & 1;

    if (bit == 0) {
      gpio_register_set_high(LED_SEG_PINS[7 - i]);
    } else {
      gpio_register_set_low(LED_SEG_PINS[7 - i]);
    }
  }
}

// BUTTON Functions
void gpio_init_btn(void) {
  IO_MUX_GPIO_REG(BTN_PIN) &= ~(7UL << IO_MUX_MCU_SEL_POS);
  IO_MUX_GPIO_REG(BTN_PIN) |=
      ((uint32_t)IO_MUX_FUNC_GPIO << IO_MUX_MCU_SEL_POS);

  IO_MUX_GPIO_REG(BTN_PIN) |= (1UL << IO_MUX_FUN_WPU_POS);
  IO_MUX_GPIO_REG(BTN_PIN) &= ~(1UL << IO_MUX_FUN_WPD_POS);
  IO_MUX_GPIO_REG(BTN_PIN) |= (1UL << IO_MUX_FUN_IE_POS);

  GPIO_ENABLE_W1TC_REG = (1UL << BTN_PIN);
}

uint32_t gpio_read(uint32_t pin) {
  return ((GPIO_IN_REG & (1UL << pin)) != 0U);
}

// --- Entry Point for ESP-IDF ---

void app_main(void) {
  printf("Hello, ESP32-S3 Tick-Based Button Logic!\n");

  gpio_init_led_seg();
  gpio_init_btn();

  uint32_t count = 0U;
  number_to_display(0U);

  while (1) {
    // Wait for initial button press (Active low: 0 = pressed)
    if (gpio_read(BTN_PIN) == 0U) {
      // 25ms Debounce
      delay_ms(DEBOUNCE_DELAY_MS);

      if (gpio_read(BTN_PIN) == 0U) {
        uint32_t elapsed_ms = 0U;
        bool long_press_triggered = false;

        // Check if button is held down for long press (800ms) using 10ms ticks
        while (gpio_read(BTN_PIN) == 0U) {
          if (elapsed_ms >= LONG_PRESS_DELAY_MS) {
            long_press_triggered = true;
            break;
          }
          delay_ms(10U);
          elapsed_ms += 10U;
        }

        if (long_press_triggered) {
          // LONG PRESS: +1 on entry, then every 500ms until release
          count = (count + 1U) % 10U;
          printf("Long press entry! Count = %lu\n", (unsigned long)count);
          number_to_display(count);

          uint32_t repeat_elapsed = 0U;

          // Stop repeating immediately on release
          while (gpio_read(BTN_PIN) == 0U) {
            if (repeat_elapsed >= REPEAT_PERIOD_MS) {
              count = (count + 1U) % 10U;
              printf("Long press repeat! Count = %lu\n", (unsigned long)count);
              number_to_display(count);
              repeat_elapsed = 0U; // Reset repeat timer
            }
            delay_ms(10U);
            repeat_elapsed += 10U;
          }
          // Debounce release and do NOT count as a click
          delay_ms(DEBOUNCE_DELAY_MS);
        } else {
          // RELEASED QUICKLY: Wait for release
          delay_ms(DEBOUNCE_DELAY_MS);
          while (gpio_read(BTN_PIN) == 0U) {
            delay_ms(10U);
          }
          delay_ms(DEBOUNCE_DELAY_MS);

          uint32_t window_elapsed = 0U;
          bool double_clicked = false;

          // 350ms Double-click window (deferred single click)
          while (window_elapsed < DOUBLE_CLICK_WINDOW_MS) {
            if (gpio_read(BTN_PIN) == 0U) {
              // Debounce second press
              delay_ms(DEBOUNCE_DELAY_MS);
              if (gpio_read(BTN_PIN) == 0U) {
                double_clicked = true;
                // Wait for second release
                while (gpio_read(BTN_PIN) == 0U) {
                  delay_ms(10U);
                }
                delay_ms(DEBOUNCE_DELAY_MS);
                break;
              }
            }
            delay_ms(10U);
            window_elapsed += 10U;
          }

          if (double_clicked) {
            // Double-click: -1 (with 0-9 wrap-around using uint32_t)
            count = (count == 0U) ? 9U : (count - 1U);
            printf("Double-click! Count = %lu\n", (unsigned long)count);
          } else {
            // Single-click: +1 (deferred until window expires with no second
            // press)
            count = (count + 1U) % 10U;
            printf("Single-click! Count = %lu\n", (unsigned long)count);
          }

          number_to_display(count);
        }
      }
    }
    delay_ms(10U);
  }
}
