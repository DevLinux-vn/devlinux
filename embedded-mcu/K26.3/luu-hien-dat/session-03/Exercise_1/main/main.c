/*
 * ESP32-S3 DevKitC-1: button-driven, one-digit 7-segment counter.
 * Display wiring assumed here: common cathode (segments turn on HIGH).
 * For a common-anode display, set DISPLAY_ACTIVE_LOW to 1.
 * Connect one current-limiting resistor to each segment; common pin to GND.
 */

#include <stdbool.h>
#include <stdint.h>

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ESP32-S3 register addresses (TRM: IO MUX and GPIO Matrix). */
#define GPIO_BASE_ADDR 0x60004000UL
#define GPIO_OUT_W1TS_REG (GPIO_BASE_ADDR + 0x0008UL)
#define GPIO_OUT_W1TC_REG (GPIO_BASE_ADDR + 0x000CUL)
#define GPIO_ENABLE_W1TS_REG (GPIO_BASE_ADDR + 0x0024UL)
#define GPIO_ENABLE_W1TC_REG (GPIO_BASE_ADDR + 0x0028UL)
#define GPIO_IN_REG (GPIO_BASE_ADDR + 0x003CUL)

#define IO_MUX_BASE_ADDR 0x60009000UL
#define IO_MUX_GPIO_REG(gpio) (IO_MUX_BASE_ADDR + 0x0004UL + (4UL * (gpio)))
#define GPIO_FUNC_OUT_SEL_CFG_BASE (GPIO_BASE_ADDR + 0x0554UL)
#define GPIO_FUNC_OUT_SEL_CFG_REG(gpio)                                        \
  (GPIO_FUNC_OUT_SEL_CFG_BASE + (4UL * (gpio)))

#define REG32(address) (*(volatile uint32_t *)(address))

/* GPIO and register-field values. */
#define GPIO_SEG_A 4U
#define GPIO_SEG_B 5U
#define GPIO_SEG_C 6U
#define GPIO_SEG_D 7U
#define GPIO_SEG_E 15U
#define GPIO_SEG_F 16U
#define GPIO_SEG_G 17U
#define GPIO_BUTTON 14U
#define GPIO_OUT_SIGNAL_INDEX 256U

#define IO_MUX_MCU_SEL_SHIFT 12U
#define IO_MUX_MCU_SEL_MASK (0x7U << IO_MUX_MCU_SEL_SHIFT)
#define IO_MUX_GPIO_FUNCTION 1U
#define IO_MUX_FUN_PD_BIT 7U
#define IO_MUX_FUN_PU_BIT 8U
#define IO_MUX_FUN_IE_BIT 9U
#define GPIO_FUNC_OUT_SEL_MASK 0x1FFU
#define GPIO_FUNC_OEN_SEL_BIT 10U

#define BUTTON_ACTIVE_LOW 1
#define DISPLAY_ACTIVE_LOW 0

#define DEBOUNCE_MS 25U
#define DOUBLE_CLICK_WINDOW_MS 350U
#define LONG_PRESS_MS 800U
#define LONG_PRESS_REPEAT_MS 500U
#define POLL_INTERVAL_MS 1U

#define SEGMENT_A_MASK (1U << 0U)
#define SEGMENT_B_MASK (1U << 1U)
#define SEGMENT_C_MASK (1U << 2U)
#define SEGMENT_D_MASK (1U << 3U)
#define SEGMENT_E_MASK (1U << 4U)
#define SEGMENT_F_MASK (1U << 5U)
#define SEGMENT_G_MASK (1U << 6U)

static const uint8_t segment_gpio[7] = {GPIO_SEG_A, GPIO_SEG_B, GPIO_SEG_C,
                                        GPIO_SEG_D, GPIO_SEG_E, GPIO_SEG_F,
                                        GPIO_SEG_G};

/* Bit order is a,b,c,d,e,f,g; a set bit means the segment is logically on. */
static const uint8_t digit_segments[10] = {
    SEGMENT_A_MASK | SEGMENT_B_MASK | SEGMENT_C_MASK | SEGMENT_D_MASK |
        SEGMENT_E_MASK | SEGMENT_F_MASK,
    SEGMENT_B_MASK | SEGMENT_C_MASK,
    SEGMENT_A_MASK | SEGMENT_B_MASK | SEGMENT_D_MASK | SEGMENT_E_MASK |
        SEGMENT_G_MASK,
    SEGMENT_A_MASK | SEGMENT_B_MASK | SEGMENT_C_MASK | SEGMENT_D_MASK |
        SEGMENT_G_MASK,
    SEGMENT_B_MASK | SEGMENT_C_MASK | SEGMENT_F_MASK | SEGMENT_G_MASK,
    SEGMENT_A_MASK | SEGMENT_C_MASK | SEGMENT_D_MASK | SEGMENT_F_MASK |
        SEGMENT_G_MASK,
    SEGMENT_A_MASK | SEGMENT_C_MASK | SEGMENT_D_MASK | SEGMENT_E_MASK |
        SEGMENT_F_MASK | SEGMENT_G_MASK,
    SEGMENT_A_MASK | SEGMENT_B_MASK | SEGMENT_C_MASK,
    SEGMENT_A_MASK | SEGMENT_B_MASK | SEGMENT_C_MASK | SEGMENT_D_MASK |
        SEGMENT_E_MASK | SEGMENT_F_MASK | SEGMENT_G_MASK,
    SEGMENT_A_MASK | SEGMENT_B_MASK | SEGMENT_C_MASK | SEGMENT_D_MASK |
        SEGMENT_F_MASK | SEGMENT_G_MASK};

static uint32_t segment_output_mask;
static uint32_t button_mask;

static void configure_pad_as_gpio(uint32_t gpio, bool input_enable,
                                  bool pull_up) {
  uint32_t value = REG32(IO_MUX_GPIO_REG(gpio));

  value = (value & ~IO_MUX_MCU_SEL_MASK) |
          (IO_MUX_GPIO_FUNCTION << IO_MUX_MCU_SEL_SHIFT);
  value &= ~(1U << IO_MUX_FUN_PD_BIT);
  value = input_enable ? value | (1U << IO_MUX_FUN_IE_BIT)
                       : value & ~(1U << IO_MUX_FUN_IE_BIT);
  value = pull_up ? value | (1U << IO_MUX_FUN_PU_BIT)
                  : value & ~(1U << IO_MUX_FUN_PU_BIT);
  REG32(IO_MUX_GPIO_REG(gpio)) = value;
}

static void configure_segment_output(uint32_t gpio) {
  configure_pad_as_gpio(gpio, false, false);

  REG32(GPIO_FUNC_OUT_SEL_CFG_REG(gpio)) =
      (GPIO_OUT_SIGNAL_INDEX & GPIO_FUNC_OUT_SEL_MASK) |
      (1U << GPIO_FUNC_OEN_SEL_BIT);
  REG32(GPIO_ENABLE_W1TS_REG) = 1U << gpio;
}

static void configure_gpio(void) {
  for (uint32_t i = 0U; i < 7U; ++i) {
    segment_output_mask |= 1U << segment_gpio[i];
    configure_segment_output(segment_gpio[i]);
  }

  button_mask = 1U << GPIO_BUTTON;
  configure_pad_as_gpio(GPIO_BUTTON, true, true);
  REG32(GPIO_ENABLE_W1TC_REG) = button_mask;
}

static void write_segments(uint8_t logical_segments) {
  uint32_t set_mask = 0U;
  uint32_t clear_mask;

  for (uint32_t i = 0U; i < 7U; ++i) {
    uint32_t pin_mask = 1U << segment_gpio[i];
    bool segment_on = (logical_segments & (1U << i)) != 0U;
    bool pin_high = DISPLAY_ACTIVE_LOW ? !segment_on : segment_on;
    if (pin_high) {
      set_mask |= pin_mask;
    }
  }

  clear_mask = segment_output_mask & ~set_mask;
  REG32(GPIO_OUT_W1TS_REG) = set_mask;
  REG32(GPIO_OUT_W1TC_REG) = clear_mask;
}

static void show_digit(uint8_t digit) { write_segments(digit_segments[digit]); }

static bool raw_button_pressed(void) {
  bool pin_high = (REG32(GPIO_IN_REG) & button_mask) != 0U;
  return BUTTON_ACTIVE_LOW ? !pin_high : pin_high;
}

static void apply_step(int8_t step, uint8_t *digit) {
  if (step > 0) {
    *digit = (*digit == 9U) ? 0U : (uint8_t)(*digit + 1U);
  } else {
    *digit = (*digit == 0U) ? 9U : (uint8_t)(*digit - 1U);
  }
  show_digit(*digit);
}

void app_main(void) {
  uint8_t digit = 0U;
  bool stable_pressed;
  bool sampled_pressed;
  bool click_pending = false;
  bool long_press_active = false;
  bool suppress_next_release_click = false;
  int64_t last_change_us;
  int64_t press_started_us = 0;
  int64_t last_click_us = 0;
  int64_t last_repeat_us = 0;

  configure_gpio();
  show_digit(digit);

  stable_pressed = raw_button_pressed();
  sampled_pressed = stable_pressed;
  last_change_us = esp_timer_get_time();

  for (;;) {
    int64_t now_us;
    bool pressed = raw_button_pressed();

    if (pressed != sampled_pressed) {
      sampled_pressed = pressed;
      last_change_us = esp_timer_get_time();
    }

    now_us = esp_timer_get_time();
    if ((sampled_pressed != stable_pressed) &&
        ((now_us - last_change_us) >= ((int64_t)DEBOUNCE_MS * 1000LL))) {
      stable_pressed = sampled_pressed;
      now_us = esp_timer_get_time();

      if (stable_pressed) {
        press_started_us = now_us;
        if (click_pending && ((now_us - last_click_us) <=
                              ((int64_t)DOUBLE_CLICK_WINDOW_MS * 1000LL))) {
          click_pending = false;
          suppress_next_release_click = true;
          apply_step(-1, &digit);
        }
        long_press_active = false;
      } else if (long_press_active) {
        long_press_active = false;
        suppress_next_release_click = false;
      } else if (suppress_next_release_click) {
        suppress_next_release_click = false;
      } else if (!click_pending) {
        click_pending = true;
        last_click_us = now_us;
      }
    }

    now_us = esp_timer_get_time();
    if (stable_pressed && !long_press_active &&
        ((now_us - press_started_us) >= ((int64_t)LONG_PRESS_MS * 1000LL))) {
      long_press_active = true;
      click_pending = false;
      apply_step(1, &digit);
      last_repeat_us = now_us;
    } else if (stable_pressed && long_press_active &&
               ((now_us - last_repeat_us) >=
                ((int64_t)LONG_PRESS_REPEAT_MS * 1000LL))) {
      apply_step(1, &digit);
      last_repeat_us = now_us;
    }

    if (click_pending && !stable_pressed &&
        ((now_us - last_click_us) >
         ((int64_t)DOUBLE_CLICK_WINDOW_MS * 1000LL))) {
      click_pending = false;
      apply_step(1, &digit);
    }

    vTaskDelay(pdMS_TO_TICKS(POLL_INTERVAL_MS));
  }
}
