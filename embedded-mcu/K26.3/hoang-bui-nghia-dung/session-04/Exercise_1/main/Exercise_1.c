#include <stdbool.h>
#include <stdint.h>


#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* =====================================================
 * 1. REGISTER DEFINITIONS
 * ===================================================== */

#define GPIO_BASE_ADDR 0x60004000UL
#define IO_MUX_BASE_ADDR 0x60009000UL

#define REG32(addr) (*(volatile uint32_t *)(addr))

/* GPIO Registers */
#define REG_GPIO_OUT_W1TS REG32(GPIO_BASE_ADDR + 0x0008U)

#define REG_GPIO_OUT_W1TC REG32(GPIO_BASE_ADDR + 0x000CU)

#define REG_GPIO_ENABLE_W1TS REG32(GPIO_BASE_ADDR + 0x0024U)

#define REG_GPIO_ENABLE_W1TC REG32(GPIO_BASE_ADDR + 0x0028U)

#define REG_GPIO_FUNC_OUT_SEL(pin)                                             \
  REG32(GPIO_BASE_ADDR + 0x0554U + ((pin) * 4U))

/* IO MUX Registers */
#define REG_IO_MUX_GPIO(pin) REG32(IO_MUX_BASE_ADDR + 0x0004U + ((pin) * 4U))

/* IO MUX Configuration */
#define IO_MUX_MCU_SEL_GPIO (1U << 12)
#define IO_MUX_FUN_DRV (2U << 10)

/* GPIO Matrix Configuration */
#define GPIO_MATRIX_SIMPLE_OUT_IDX 0x100U
#define GPIO_MATRIX_OEN_FROM_GPIO (1U << 10)

/* =====================================================
 * 2. GPIO DEFINITIONS
 * ===================================================== */

#define DISPLAY_COMMON_ANODE 0U

#define PIN_SEG_A 4U
#define PIN_SEG_B 5U
#define PIN_SEG_C 6U
#define PIN_SEG_D 7U
#define PIN_SEG_E 15U
#define PIN_SEG_F 16U
#define PIN_SEG_G 17U
#define PIN_SEG_DP 18U

#define PIN_BUTTON GPIO_NUM_14

#define GPIO_BIT(pin) (1UL << (pin))

#define ALL_SEGS_MASK                                                          \
  (GPIO_BIT(PIN_SEG_A) | GPIO_BIT(PIN_SEG_B) | GPIO_BIT(PIN_SEG_C) |           \
   GPIO_BIT(PIN_SEG_D) | GPIO_BIT(PIN_SEG_E) | GPIO_BIT(PIN_SEG_F) |           \
   GPIO_BIT(PIN_SEG_G) | GPIO_BIT(PIN_SEG_DP))

/* =====================================================
 * 3. TIMING DEFINITIONS
 * ===================================================== */

#define DEBOUNCE_US 25000LL
#define DOUBLE_WINDOW_US 350000LL
#define LONG_PRESS_US 800000LL
#define LONG_REPEAT_US 500000LL

#define BUTTON_POLL_INTERVAL_MS 10U

#define DIGIT_COUNT 10U
#define SEGMENT_COUNT 7U

/* =====================================================
 * 4. SEVEN-SEGMENT LOOKUP TABLE
 * ===================================================== */

/* Bit 0 -> 6: segment a -> g */
static const uint8_t s_digit_patterns[DIGIT_COUNT] = {
    0x3FU, // 0
    0x06U, // 1
    0x5BU, // 2
    0x4FU, // 3
    0x66U, // 4
    0x6DU, // 5
    0x7DU, // 6
    0x07U, // 7
    0x7FU, // 8
    0x6FU  // 9
};

static const uint8_t s_segment_pins[SEGMENT_COUNT] = {
    PIN_SEG_A, PIN_SEG_B, PIN_SEG_C, PIN_SEG_D,
    PIN_SEG_E, PIN_SEG_F, PIN_SEG_G};

/* =====================================================
 * 5. SEVEN-SEGMENT DISPLAY
 * ===================================================== */

static uint32_t compute_segment_mask(uint8_t pattern) {
  uint32_t mask = 0U;

  for (uint32_t i = 0; i < SEGMENT_COUNT; i++) {
    if ((pattern & (1U << i)) != 0U) {
      mask |= GPIO_BIT(s_segment_pins[i]);
    }
  }
  return mask;
}

static void display_digit(uint8_t digit) {
  if (digit >= DIGIT_COUNT) {
    return;
  }
  const uint32_t active_mask = compute_segment_mask(s_digit_patterns[digit]);

#if DISPLAY_COMMON_ANODE

  /* Turn OFF all segments */
  REG_GPIO_OUT_W1TS = ALL_SEGS_MASK;

  /* Turn ON selected segments */
  REG_GPIO_OUT_W1TC = active_mask;

#else

  /* Turn OFF all segments */
  REG_GPIO_OUT_W1TC = ALL_SEGS_MASK;

  /* Turn ON selected segments */
  REG_GPIO_OUT_W1TS = active_mask;

#endif
}

static void display_init(void) {
  static const uint8_t output_pins[] = {PIN_SEG_A, PIN_SEG_B, PIN_SEG_C,
                                        PIN_SEG_D, PIN_SEG_E, PIN_SEG_F,
                                        PIN_SEG_G, PIN_SEG_DP};

  /* Disable GPIO outputs */
  REG_GPIO_ENABLE_W1TC = ALL_SEGS_MASK;

  /* Initialize all segments to OFF */
#if DISPLAY_COMMON_ANODE

  REG_GPIO_OUT_W1TS = ALL_SEGS_MASK;

#else

  REG_GPIO_OUT_W1TC = ALL_SEGS_MASK;

#endif

  /* Configure IO MUX and GPIO Matrix */
  for (uint32_t i = 0; i < sizeof(output_pins) / sizeof(output_pins[0]); i++) {

    uint8_t pin = output_pins[i];

    REG_IO_MUX_GPIO(pin) = IO_MUX_MCU_SEL_GPIO | IO_MUX_FUN_DRV;

    REG_GPIO_FUNC_OUT_SEL(pin) =
        GPIO_MATRIX_SIMPLE_OUT_IDX | GPIO_MATRIX_OEN_FROM_GPIO;
  }

  /* Enable GPIO outputs */
  REG_GPIO_ENABLE_W1TS = ALL_SEGS_MASK;
}

/* =====================================================
 * 6. BUTTON SHARED VARIABLES
 * ===================================================== */

/* TODO */
static volatile bool s_pressed = false; // lưu trạng thái nút nhấn

static volatile int64_t s_press_us = 0;   // thời gian nút được nhấn
static volatile int64_t s_release_us = 0; // thời gian nút được nhả
static portMUX_TYPE s_lock =
    portMUX_INITIALIZER_UNLOCKED; // đồng bộ dữ liệu giữa ISR và task chính

/* =====================================================
 * 7. BUTTON INTERRUPT SERVICE ROUTINE
 * ===================================================== */

/* TODO */
static void IRAM_ATTR button_isr(void *arg) {
  (void)arg;
  bool pressed = (gpio_get_level(PIN_BUTTON) == 0); // đọc nút nhấn
  int64_t now = esp_timer_get_time();

  portENTER_CRITICAL_ISR(
      &s_lock); // lưu thời điểm xảy ra cạnh & cập nhật dữ liệu chung

  s_pressed = pressed;

  if (pressed) {
    s_press_us = now;
  } else {
    s_release_us = now;
  }

  portEXIT_CRITICAL_ISR(&s_lock);
}

/* =====================================================
 * 8. BUTTON GPIO CONFIGURATION
 * ===================================================== */

/* TODO */
static void button_init(void) {
  gpio_config_t button_config = {
      .pin_bit_mask = (1ULL << PIN_BUTTON),
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type =
          GPIO_INTR_ANYEDGE // Dùng ANYEDGE để phát hiện ngắt khi nhân và nhả
  };

  ESP_ERROR_CHECK(gpio_config(&button_config));

  ESP_ERROR_CHECK(gpio_install_isr_service(0));

  ESP_ERROR_CHECK(gpio_isr_handler_add(PIN_BUTTON, button_isr, NULL));
}
/* =====================================================
 * 9. BUTTON FSM
 * ===================================================== */

/* TODO */
typedef struct {
  bool pressed;
  int64_t press_us;
  int64_t release_us;
} button_snapshot_t;
// Lưu các thời điểm vào snapshot
static button_snapshot_t button_get_snapshot(void) {
  button_snapshot_t snapshot;
  portENTER_CRITICAL(&s_lock);
  snapshot.pressed = s_pressed;
  snapshot.press_us = s_press_us;
  snapshot.release_us = s_release_us;
  portEXIT_CRITICAL(&s_lock);

  return snapshot;
}
/* Máy trạng thái của nút nhấn  */
typedef enum {
  STATE_IDLE,
  STATE_PRESSED,
  STATE_WAIT_DOUBLE,
  STATE_LONG_PRESS
} button_state_t;
/* Máy trạng thái của các sự kiện */
typedef enum {
  BTN_EVENT_NONE,
  BTN_EVENT_SINGLE_CLICK,
  BTN_EVENT_DOUBLE_CLICK,
  BTN_EVENT_LONG_PRESS_TICK
} button_event_t;

typedef struct {
  button_state_t state;

  bool stable_pressed;
  uint8_t click_count;

  int64_t press_anchor;
  int64_t release_anchor;
  int64_t repeat_anchor;
} button_fsm_t;

static button_fsm_t s_fsm = {
    .state = STATE_IDLE, .stable_pressed = false, .click_count = 0};
// Debounce button using timestamps saved by ISR
static bool button_debounce(const button_snapshot_t *snapshot, int64_t now) {
  int64_t last_edge_us =
      snapshot->pressed ? snapshot->press_us : snapshot->release_us;

  if (snapshot->pressed != s_fsm.stable_pressed &&
      now - last_edge_us >= DEBOUNCE_US) {
    s_fsm.stable_pressed = snapshot->pressed;
    return true;
  }

  return false;
}

// FSM

static button_event_t button_fsm_update(const button_snapshot_t *snapshot,
                                        int64_t now) {
  button_event_t evt = BTN_EVENT_NONE;
  bool changed = button_debounce(snapshot, now);
  bool pressed = s_fsm.stable_pressed;

  switch (s_fsm.state) {
  case STATE_IDLE:
    if (changed && pressed) {
      s_fsm.click_count = 1;
      s_fsm.press_anchor = snapshot->press_us;
      s_fsm.state = STATE_PRESSED;
    }
    break;

  case STATE_PRESSED:
    if (changed && !pressed) {
      if (s_fsm.click_count == 2) {
        evt = BTN_EVENT_DOUBLE_CLICK;
        s_fsm.click_count = 0;
        s_fsm.state = STATE_IDLE;
      } else {
        s_fsm.release_anchor = snapshot->release_us;
        s_fsm.state = STATE_WAIT_DOUBLE;
      }
    } else if (s_fsm.click_count == 1 && pressed && snapshot->pressed &&
               now - s_fsm.press_anchor >= LONG_PRESS_US) {
      s_fsm.repeat_anchor = s_fsm.press_anchor + LONG_PRESS_US;
      s_fsm.state = STATE_LONG_PRESS;
      evt = BTN_EVENT_LONG_PRESS_TICK;
    }
    break;
  case STATE_WAIT_DOUBLE:
    if (snapshot->pressed &&
        snapshot->press_us - snapshot->release_us < DOUBLE_WINDOW_US &&
        snapshot->press_us >= s_fsm.release_anchor) {

      if (changed && pressed) {
        s_fsm.click_count = 2;
        s_fsm.press_anchor = snapshot->press_us;
        s_fsm.state = STATE_PRESSED;
      }

    } else if (now - s_fsm.release_anchor >= DOUBLE_WINDOW_US) {

      evt = BTN_EVENT_SINGLE_CLICK;
      s_fsm.click_count = 0;
      s_fsm.state = STATE_IDLE;

      if (changed && pressed) {
        s_fsm.click_count = 1;
        s_fsm.press_anchor = snapshot->press_us;
        s_fsm.state = STATE_PRESSED;
      }
    }
    break;
  case STATE_LONG_PRESS:
    if (changed && !pressed) {
      s_fsm.click_count = 0;
      s_fsm.state = STATE_IDLE;
    } else if (pressed && snapshot->pressed &&
               now - s_fsm.repeat_anchor >= LONG_PRESS_US) {
      s_fsm.repeat_anchor += LONG_PRESS_US;
      evt = BTN_EVENT_LONG_PRESS_TICK;
    }
    break;
  default:
    s_fsm.click_count = 0;
    s_fsm.state = STATE_IDLE;
    break;
  }
  return evt;
}
/* =====================================================
 * 10. MAIN APPLICATION
 * ===================================================== */

void app_main(void) {
  display_init();
  uint8_t counter = 0U;
  display_digit(counter);

  /* TODO: button_init(); */
  button_init();

  while (true) {

    /* TODO: Read ISR snapshot */
    button_snapshot_t snapshot = button_get_snapshot();
    uint64_t now = esp_timer_get_time();

    /* TODO: Debounce */
    button_event_t evt = button_fsm_update(&snapshot, now);
    /* TODO: Gesture FSM */
    switch (evt) {
    case BTN_EVENT_SINGLE_CLICK:
    case BTN_EVENT_LONG_PRESS_TICK:
      counter = (counter + 1U) % DIGIT_COUNT;
      display_digit(counter);
      break;
    case BTN_EVENT_DOUBLE_CLICK:
      counter = (counter - 1U) % DIGIT_COUNT;
      display_digit(counter);
      break;
    default:
      break;
    }
    /* TODO: Update counter */

    vTaskDelay(pdMS_TO_TICKS(BUTTON_POLL_INTERVAL_MS));
  }
}
