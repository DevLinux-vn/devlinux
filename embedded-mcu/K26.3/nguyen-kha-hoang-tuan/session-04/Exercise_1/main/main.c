#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// ============================================================================
// 1. 7-SEGMENT DISPLAY USING ESP-IDF GPIO DRIVER API
// ============================================================================

const uint32_t LED_SEG_PINS[8] = {4U, 5U, 6U, 7U, 15U, 16U, 17U, 18U};

const uint32_t SEG_BITS[10] = {
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

void gpio_init_led_seg(void) {
  uint64_t pin_bit_mask = 0ULL;
  for (int i = 0; i < 8; ++i) {
    pin_bit_mask |= (1ULL << LED_SEG_PINS[i]);
  }

  gpio_config_t io_conf = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = pin_bit_mask,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .pull_up_en = GPIO_PULLUP_DISABLE,
  };
  gpio_config(&io_conf);

  // Set initial HIGH state (all active-low segments OFF)
  for (int i = 0; i < 8; ++i) {
    gpio_set_level(LED_SEG_PINS[i], 1);
  }
}

void number_to_display(uint32_t digit) {
  uint32_t index = digit % 10U;
  uint32_t digit_mask = SEG_BITS[index];

  for (int i = 7; i >= 0; i--) {
    uint32_t bit = (digit_mask >> i) & 1U;
    // Active-low segment logic: 0 = ON, 1 = OFF
    gpio_set_level(LED_SEG_PINS[7 - i], (bit == 0U) ? 1 : 0);
  }
}

// ============================================================================
// 2. BUTTON CONFIGURATION & ISR SHARED STATE (ESP-IDF GPIO DRIVER API)
// ============================================================================

#define BTN_PIN                (14U)
#define DEBOUNCE_DELAY_US      (25000LL)  // 25 ms in microseconds
#define DOUBLE_CLICK_WINDOW_US (350000LL) // 350 ms in microseconds
#define LONG_PRESS_DELAY_US    (800000LL) // 800 ms in microseconds
#define REPEAT_PERIOD_US       (500000LL) // 500 ms in microseconds

// Shared volatile state updated ONLY inside the ISR
static volatile int     g_isr_btn_level  = 1; // 1 = released, 0 = pressed
static volatile int64_t g_isr_press_ts   = 0; // Timestamp (us) of last 1 -> 0 edge
static volatile int64_t g_isr_release_ts = 0; // Timestamp (us) of last 0 -> 1 edge

// Spinlock to synchronize 64-bit timestamps and multi-field accesses
static portMUX_TYPE g_btn_spinlock = portMUX_INITIALIZER_UNLOCKED;

// ISR Handler: Triggered on ANYEDGE (press 1->0 and release 0->1)
// Strictly reads button level, timestamps the edge, and saves state.
static void IRAM_ATTR gpio_isr_handler(void *arg) {
  uint32_t gpio_num = (uint32_t)arg;
  int level = gpio_get_level(gpio_num); // ONLY place the button pin is read!
  int64_t now_us = esp_timer_get_time();

  portENTER_CRITICAL_ISR(&g_btn_spinlock);
  g_isr_btn_level = level;
  if (level == 0) {
    g_isr_press_ts = now_us;
  } else {
    g_isr_release_ts = now_us;
  }
  portEXIT_CRITICAL_ISR(&g_btn_spinlock);
}

void gpio_init_btn(void) {
  gpio_config_t io_conf = {
      .intr_type = GPIO_INTR_ANYEDGE, // Capture both press (1->0) and release (0->1)
      .mode = GPIO_MODE_INPUT,
      .pin_bit_mask = (1ULL << BTN_PIN),
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .pull_up_en = GPIO_PULLUP_ENABLE,
  };
  gpio_config(&io_conf);

  gpio_install_isr_service(0);
  gpio_isr_handler_add(BTN_PIN, gpio_isr_handler, (void *)BTN_PIN);
}

// ============================================================================
// 3. NON-BLOCKING GESTURE DECODER IN app_main()
// ============================================================================

typedef enum {
  STATE_IDLE = 0,
  STATE_FIRST_PRESS,
  STATE_LONG_HOLD,
  STATE_WAIT_DOUBLE_CLICK,
  STATE_SECOND_PRESS
} btn_fsm_state_t;

void app_main(void) {
  printf("Hello, ESP32-S3 Non-Blocking ANYEDGE Button Logic!\n");

  gpio_init_led_seg();
  gpio_init_btn();

  uint32_t count = 0U;
  number_to_display(count);

  // Debounced state tracked by app_main()
  int debounced_level = 1;       // 1 = released, 0 = pressed
  int64_t stable_press_ts = 0;   // Timestamp of last debounced press
  int64_t stable_release_ts = 0; // Timestamp of last debounced release
  int64_t last_repeat_ts = 0;    // Timestamp for 500ms auto-repeat

  btn_fsm_state_t state = STATE_IDLE;

  while (1) {
    // 1. Take a consistent atomic snapshot of the ISR's volatile variables
    int snap_level;
    int64_t snap_press_ts;
    int64_t snap_release_ts;

    portENTER_CRITICAL(&g_btn_spinlock);
    snap_level = g_isr_btn_level;
    snap_press_ts = g_isr_press_ts;
    snap_release_ts = g_isr_release_ts;
    portEXIT_CRITICAL(&g_btn_spinlock);

    int64_t now_us = esp_timer_get_time();

    // 2. Debounce the saved edges
    int64_t latest_edge_ts = (snap_level == 0) ? snap_press_ts : snap_release_ts;

    bool press_event = false;
    bool release_event = false;

    if (snap_level != debounced_level &&
        latest_edge_ts > 0 &&
        (now_us - latest_edge_ts) >= DEBOUNCE_DELAY_US) {
      debounced_level = snap_level;
      if (debounced_level == 0) {
        press_event = true;
        stable_press_ts = snap_press_ts;
      } else {
        release_event = true;
        stable_release_ts = snap_release_ts;
      }
    }

    // 3. Non-Blocking Finite State Machine for Gesture Decoding
    switch (state) {
    case STATE_IDLE:
      if (press_event) {
        state = STATE_FIRST_PRESS;
      }
      break;

    case STATE_FIRST_PRESS:
      if (release_event) {
        // Released before 800ms -> wait up to 350ms for a second click
        state = STATE_WAIT_DOUBLE_CLICK;
      } else if (debounced_level == 0 &&
                 (now_us - stable_press_ts) >= LONG_PRESS_DELAY_US) {
        // Held >= 800ms -> Long Press entry (+1)
        count = (count + 1U) % 10U;
        printf("Long press entry! Count = %lu\n", (unsigned long)count);
        number_to_display(count);

        last_repeat_ts = now_us;
        state = STATE_LONG_HOLD;
      }
      break;

    case STATE_LONG_HOLD:
      if (release_event) {
        // Release ends the hold and MUST NOT add a click
        state = STATE_IDLE;
      } else if (debounced_level == 0 &&
                 (now_us - last_repeat_ts) >= REPEAT_PERIOD_US) {
        // Continue repeating every 500ms without needing new edges
        count = (count + 1U) % 10U;
        printf("Long press repeat! Count = %lu\n", (unsigned long)count);
        number_to_display(count);

        last_repeat_ts = now_us;
      }
      break;

    case STATE_WAIT_DOUBLE_CLICK:
      if (press_event) {
        // Second press within 350ms window -> Double-Click (-1)
        count = (count == 0U) ? 9U : (count - 1U);
        printf("Double-click! Count = %lu\n", (unsigned long)count);
        number_to_display(count);

        state = STATE_SECOND_PRESS;
      } else if ((now_us - stable_release_ts) >= DOUBLE_CLICK_WINDOW_US) {
        // 350ms elapsed with no second press -> Single-Click (+1)
        count = (count + 1U) % 10U;
        printf("Single-click! Count = %lu\n", (unsigned long)count);
        number_to_display(count);

        state = STATE_IDLE;
      }
      break;

    case STATE_SECOND_PRESS:
      // Wait for the second press of the double-click to be released cleanly
      if (release_event) {
        state = STATE_IDLE;
      }
      break;
    }

    // 4. End each iteration with a 10ms non-blocking delay
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
