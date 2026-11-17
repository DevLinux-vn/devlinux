
#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

/* ESP32-S3 register addresses (TRM: IO MUX and GPIO Matrix). */
#define GPIO_BASE_ADDR 0x60004000UL
#define GPIO_OUT_W1TS_REG (GPIO_BASE_ADDR + 0x0008UL)
#define GPIO_OUT_W1TC_REG (GPIO_BASE_ADDR + 0x000CUL)
#define GPIO_ENABLE_W1TS_REG (GPIO_BASE_ADDR + 0x0024UL)
#define GPIO_ENABLE_W1TC_REG (GPIO_BASE_ADDR + 0x0028UL)

#define IO_MUX_BASE_ADDR 0x60009000UL
#define IO_MUX_GPIO_REG(gpio) (IO_MUX_BASE_ADDR + 0x0004UL + (4UL * (gpio)))
#define GPIO_FUNC_OUT_SEL_CFG_BASE (GPIO_BASE_ADDR + 0x0554UL)
#define GPIO_FUNC_OUT_SEL_CFG_REG(gpio)                                        \
  (GPIO_FUNC_OUT_SEL_CFG_BASE + (4UL * (gpio)))
#define REG32(address) (*(volatile uint32_t *)(address))

/* GPIO assignments from the Session 03 circuit. */
#define GPIO_SEG_A 4U
#define GPIO_SEG_B 5U
#define GPIO_SEG_C 6U
#define GPIO_SEG_D 7U
#define GPIO_SEG_E 15U
#define GPIO_SEG_F 16U
#define GPIO_SEG_G 17U
#define GPIO_BUTTON GPIO_NUM_14
#define GPIO_BUTTON_NUMBER 14U
#define GPIO_OUT_SIGNAL_INDEX 256U

#define IO_MUX_MCU_SEL_SHIFT 12U
#define IO_MUX_MCU_SEL_MASK (0x7U << IO_MUX_MCU_SEL_SHIFT)
#define IO_MUX_GPIO_FUNCTION 1U
#define IO_MUX_FUN_PD_BIT 7U
#define IO_MUX_FUN_PU_BIT 8U
#define IO_MUX_FUN_IE_BIT 9U
#define GPIO_FUNC_OUT_SEL_MASK 0x1FFU
#define GPIO_FUNC_OEN_SEL_BIT 10U

#define DISPLAY_ACTIVE_LOW 0
#define DEBOUNCE_MS 25LL
#define DOUBLE_CLICK_WINDOW_MS 350LL
#define LONG_PRESS_MS 800LL
#define LONG_PRESS_REPEAT_MS 500LL
#define LOOP_DELAY_MS 10U

#define SEGMENT_A_MASK (1U << 0U)
#define SEGMENT_B_MASK (1U << 1U)
#define SEGMENT_C_MASK (1U << 2U)
#define SEGMENT_D_MASK (1U << 3U)
#define SEGMENT_E_MASK (1U << 4U)
#define SEGMENT_F_MASK (1U << 5U)
#define SEGMENT_G_MASK (1U << 6U)

#define MS_TO_US(milliseconds) ((int64_t)(milliseconds) * 1000LL)

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

/* ISR-to-main shared state. The critical section also makes 64-bit copies
 * atomic. */
static portMUX_TYPE button_state_mux = portMUX_INITIALIZER_UNLOCKED;
static volatile bool button_pressed_isr;
static volatile int64_t last_press_edge_us;
static volatile int64_t last_release_edge_us;
static volatile uint32_t button_edge_sequence;

typedef struct {
  bool pressed;
  int64_t press_edge_us;
  int64_t release_edge_us;
  uint32_t edge_sequence;
} button_snapshot_t;

static uint32_t segment_output_mask;

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

static void configure_display_pin(uint32_t gpio) {
  configure_pad_as_gpio(gpio, false, false);
  REG32(GPIO_FUNC_OUT_SEL_CFG_REG(gpio)) =
      (GPIO_OUT_SIGNAL_INDEX & GPIO_FUNC_OUT_SEL_MASK) |
      (1U << GPIO_FUNC_OEN_SEL_BIT);
  REG32(GPIO_ENABLE_W1TS_REG) = 1U << gpio;
}

static void configure_display(void) {
  for (uint32_t i = 0U; i < 7U; ++i) {
    segment_output_mask |= 1U << segment_gpio[i];
    configure_display_pin(segment_gpio[i]);
  }
}

static void write_segments(uint8_t logical_segments) {
  uint32_t set_mask = 0U;

  for (uint32_t i = 0U; i < 7U; ++i) {
    bool segment_on = (logical_segments & (1U << i)) != 0U;
    bool pin_high = DISPLAY_ACTIVE_LOW ? !segment_on : segment_on;
    if (pin_high) {
      set_mask |= 1U << segment_gpio[i];
    }
  }

  REG32(GPIO_OUT_W1TS_REG) = set_mask;
  REG32(GPIO_OUT_W1TC_REG) = segment_output_mask & ~set_mask;
}

static void show_digit(uint8_t digit) { write_segments(digit_segments[digit]); }

static void apply_step(int8_t step, uint8_t *digit) {
  if (step > 0) {
    *digit = (*digit == 9U) ? 0U : (uint8_t)(*digit + 1U);
  } else {
    *digit = (*digit == 0U) ? 9U : (uint8_t)(*digit - 1U);
  }
  show_digit(*digit);
}

/* Keep the ISR short: sample the pin, timestamp the edge, and save the
 * snapshot. */
static void IRAM_ATTR button_isr_handler(void *arg) {
  int level = gpio_get_level(GPIO_BUTTON);
  int64_t edge_time_us = esp_timer_get_time();
  bool pressed = (level == 0);

  (void)arg;
  portENTER_CRITICAL_ISR(&button_state_mux);
  button_pressed_isr = pressed;
  if (pressed) {
    last_press_edge_us = edge_time_us;
  } else {
    last_release_edge_us = edge_time_us;
  }
  ++button_edge_sequence;
  portEXIT_CRITICAL_ISR(&button_state_mux);
}

static void configure_button_interrupt(void) {
  const gpio_config_t button_config = {.pin_bit_mask = 1ULL
                                                       << GPIO_BUTTON_NUMBER,
                                       .mode = GPIO_MODE_INPUT,
                                       .pull_up_en = GPIO_PULLUP_ENABLE,
                                       .pull_down_en = GPIO_PULLDOWN_DISABLE,
                                       .intr_type = GPIO_INTR_ANYEDGE};

  ESP_ERROR_CHECK(gpio_config(&button_config));
  ESP_ERROR_CHECK(gpio_install_isr_service(0));
  ESP_ERROR_CHECK(gpio_isr_handler_add(GPIO_BUTTON, button_isr_handler, NULL));
}

static button_snapshot_t get_button_snapshot(void) {
  button_snapshot_t snapshot;

  portENTER_CRITICAL(&button_state_mux);
  snapshot.pressed = button_pressed_isr;
  snapshot.press_edge_us = last_press_edge_us;
  snapshot.release_edge_us = last_release_edge_us;
  snapshot.edge_sequence = button_edge_sequence;
  portEXIT_CRITICAL(&button_state_mux);

  return snapshot;
}

void app_main(void) {
  uint8_t digit = 0U;
  bool stable_pressed = false;
  bool candidate_pressed = false;
  bool click_pending = false;
  bool long_press_active = false;
  bool suppress_release_click = false;
  uint32_t seen_edge_sequence = 0U;
  int64_t candidate_edge_us = 0;
  int64_t press_started_us = 0;
  int64_t last_click_release_us = 0;
  int64_t last_repeat_us = 0;

  configure_display();
  show_digit(digit);
  configure_button_interrupt();

  for (;;) {
    button_snapshot_t snapshot = get_button_snapshot();
    int64_t now_us;

    /* A newer saved edge starts or restarts the software debounce interval. */
    if (snapshot.edge_sequence != seen_edge_sequence) {
      seen_edge_sequence = snapshot.edge_sequence;
      candidate_pressed = snapshot.pressed;
      candidate_edge_us =
          candidate_pressed ? snapshot.press_edge_us : snapshot.release_edge_us;
    }

    now_us = esp_timer_get_time();
    if ((candidate_pressed != stable_pressed) &&
        (now_us - candidate_edge_us >= MS_TO_US(DEBOUNCE_MS))) {
      stable_pressed = candidate_pressed;

      if (stable_pressed) {
        press_started_us = candidate_edge_us;

        if (click_pending) {
          if (candidate_edge_us - last_click_release_us <=
              MS_TO_US(DOUBLE_CLICK_WINDOW_MS)) {
            click_pending = false;
            suppress_release_click = true;
            apply_step(-1, &digit);
          } else {
            /* The next press came too late; finish the first click. */
            click_pending = false;
            apply_step(1, &digit);
          }
        }
      } else if (long_press_active) {
        long_press_active = false;
        suppress_release_click = false;
      } else if (suppress_release_click) {
        suppress_release_click = false;
      } else if (!click_pending) {
        click_pending = true;
        last_click_release_us = candidate_edge_us;
      }
    }

    now_us = esp_timer_get_time();
    if (stable_pressed && !long_press_active &&
        (now_us - press_started_us >= MS_TO_US(LONG_PRESS_MS))) {
      long_press_active = true;
      click_pending = false;
      apply_step(1, &digit);
      last_repeat_us = now_us;
    } else if (stable_pressed && long_press_active &&
               (now_us - last_repeat_us >= MS_TO_US(LONG_PRESS_REPEAT_MS))) {
      apply_step(1, &digit);
      last_repeat_us = now_us;
    }

    if (click_pending && !stable_pressed &&
        (now_us - last_click_release_us > MS_TO_US(DOUBLE_CLICK_WINDOW_MS))) {
      click_pending = false;
      apply_step(1, &digit);
    }

    vTaskDelay(pdMS_TO_TICKS(LOOP_DELAY_MS));
  }
}
