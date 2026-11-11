/*
 * ESP32-S3 / ESP-IDF v5.2 — LED 7 đoạn ANODE CHUNG.
 * COM -> 3V3. a,b,c,d,e,f,g -> GPIO4,5,6,7,15,16,17.
 * Mỗi đoạn có điện trở riêng 220–330 ohm (mạch hiện tại: 300 ohm).
 * Nút: GPIO14 -> GND, dùng pull-up nội. DP không nối.
 * GPIO chỉ truy cập thanh ghi; không dùng driver/gpio.h hay API gpio_*.
 * Xem HUONG_DAN.md để biết nguồn địa chỉ, trường bit và cách kiểm tra.
 */
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

/* Địa chỉ nền lấy từ memory map ESP32-S3 / reg_base.h của IDF v5.2. */
#define GPIO_BASE                  0x60004000UL
#define IO_MUX_BASE                0x60009000UL
/* * truy cập nội dung; volatile giữ các lần đọc/ghi phần cứng; uint32_t: 32 bit. */
#define REG32(address)             (*(volatile uint32_t *)(uintptr_t)(address))
#define BIT32(n)                   (1UL << (n))
#define IO_MUX_ADDR(n)             (IO_MUX_BASE + 0x04UL + 4UL * (n))
#define GPIO_PIN_ADDR(n)           (GPIO_BASE + 0x74UL + 4UL * (n))
#define GPIO_ROUTE_ADDR(n)         (GPIO_BASE + 0x554UL + 4UL * (n))
#define OUT_SET_ADDR               (GPIO_BASE + 0x08UL)
#define OUT_CLEAR_ADDR             (GPIO_BASE + 0x0CUL)
#define ENABLE_SET_ADDR            (GPIO_BASE + 0x24UL)
#define ENABLE_CLEAR_ADDR          (GPIO_BASE + 0x28UL)
#define INPUT_ADDR                 (GPIO_BASE + 0x3CUL)

#define MUX_FUNCTION_SHIFT         12U
#define MUX_FUNCTION_MASK          (7UL << MUX_FUNCTION_SHIFT)
#define MUX_GPIO_FUNCTION          1UL
#define MUX_DRIVE_SHIFT            10U
#define MUX_DRIVE_MASK             (3UL << MUX_DRIVE_SHIFT)
#define MUX_INPUT_ENABLE           BIT32(9)
#define MUX_PULLUP                 BIT32(8)
#define MUX_PULLDOWN               BIT32(7)
#define PIN_OPEN_DRAIN             BIT32(2)
#define ROUTE_SIGNAL_MASK          0x1FFUL
#define ROUTE_GPIO_SIGNAL          256UL
#define ROUTE_INVERT               BIT32(9)
#define ROUTE_SOFTWARE_ENABLE      BIT32(10)
#define ROUTE_ENABLE_INVERT        BIT32(11)

#define BUTTON_PIN                 14U
#define SEGMENT_COUNT              7U
#define DISPLAY_COMMON_ANODE       1
#define DEBOUNCE_MS                25ULL
#define DOUBLE_CLICK_MS            350ULL
#define LONG_PRESS_MS              800ULL
#define REPEAT_MS                  500ULL
#define MICROSECONDS_PER_MS        1000ULL
#define DIGIT_COUNT                10U

static const uint8_t segment_pins[SEGMENT_COUNT] = {4, 5, 6, 7, 15, 16, 17};
/* bit 0=a, 1=b, ..., 6=g; 1 có nghĩa là đoạn CẦN SÁNG, chưa phải mức GPIO. */
static const uint8_t digit_table[DIGIT_COUNT] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};
static uint32_t segment_mask;
static uint8_t counter;

/* Xóa đúng các bit trong mask, rồi chèn giá trị mới; giữ nguyên bit khác.
 * Chỉ dùng cho thanh ghi R/W, không dùng với thanh ghi lệnh W1TS/W1TC.
 */
static void update_field(uint32_t address, uint32_t mask, uint32_t value)
{
    uint32_t old_value = REG32(address);
    REG32(address) = (old_value & ~mask) | (value & mask);
}

static void display_digit(uint8_t digit)
{
    uint32_t lit_pins = 0;
    for (uint32_t i = 0; i < SEGMENT_COUNT; ++i) {
        if ((digit_table[digit] & BIT32(i)) != 0) {
            lit_pins |= BIT32(segment_pins[i]);
        }
    }
    /* Anode chung: đoạn sáng cần LOW; cathode chung: đoạn sáng cần HIGH.
     * XOR với mặt nạ chỉ đảo bảy chân LED, không tác động chân khác.
     */
    uint32_t high_pins = lit_pins;
    if (DISPLAY_COMMON_ANODE) {
        high_pins = lit_pins ^ segment_mask;
    }
    /* W1TS/W1TC: ghi 1 để ra lệnh, ghi 0 không tác động. Dùng =, không |=. */
    REG32(OUT_SET_ADDR) = high_pins;
    REG32(OUT_CLEAR_ADDR) = segment_mask & ~high_pins;
}

static void counter_up(void)
{
    counter = (uint8_t)((counter + 1U) % DIGIT_COUNT);
    display_digit(counter);
}

static void counter_down(void)
{
    /* +9 tương đương -1 theo modulo 10, tránh kết quả âm khi đang ở 0. */
    counter = (uint8_t)((counter + DIGIT_COUNT - 1U) % DIGIT_COUNT);
    display_digit(counter);
}

static void hardware_init(void)
{
    for (uint32_t i = 0; i < SEGMENT_COUNT; ++i) {
        segment_mask |= BIT32(segment_pins[i]);
    }
    /* Tạm tắt đầu ra trong lúc chuẩn bị, tránh xuất nhầm mức. */
    REG32(ENABLE_CLEAR_ADDR) = segment_mask | BIT32(BUTTON_PIN);

    for (uint32_t i = 0; i < SEGMENT_COUNT; ++i) {
        uint32_t pin = segment_pins[i];
        /* Chọn sức lái xấp xỉ 10mA theo TRM: GPIO17/18 có mã khác.
         * Đây là khả năng lái, KHÔNG phải nguồn dòng cố định 10mA.
         */
         uint32_t drive_code = (pin == 17U || pin == 18U) ? 2UL : 1UL;
        uint32_t mask = MUX_FUNCTION_MASK | MUX_DRIVE_MASK |
                        MUX_INPUT_ENABLE | MUX_PULLUP | MUX_PULLDOWN;
        uint32_t value = (MUX_GPIO_FUNCTION << MUX_FUNCTION_SHIFT) |
                         (drive_code << MUX_DRIVE_SHIFT);
        update_field(IO_MUX_ADDR(pin), mask, value);
        update_field(GPIO_PIN_ADDR(pin), PIN_OPEN_DRAIN, 0); /* push-pull */
        /* Signal 256 lấy dữ liệu từ GPIO_OUT; OEN_SEL=1 lấy enable từ
         * GPIO_ENABLE. Không đảo dữ liệu hoặc tín hiệu cho phép xuất.
         */
        update_field(GPIO_ROUTE_ADDR(pin),
                     ROUTE_SIGNAL_MASK | ROUTE_INVERT |
                     ROUTE_SOFTWARE_ENABLE | ROUTE_ENABLE_INVERT,
                     ROUTE_GPIO_SIGNAL | ROUTE_SOFTWARE_ENABLE);
    }

    update_field(IO_MUX_ADDR(BUTTON_PIN),
                 MUX_FUNCTION_MASK | MUX_INPUT_ENABLE | MUX_PULLUP | MUX_PULLDOWN,
                 (MUX_GPIO_FUNCTION << MUX_FUNCTION_SHIFT) |
                 MUX_INPUT_ENABLE | MUX_PULLUP);

    counter = 0;
    display_digit(counter);                 /* Chuẩn bị số 0 trước. */
    REG32(ENABLE_SET_ADDR) = segment_mask;   /* Sau đó mới bật đầu ra. */
}

/* Tất cả thời gian tính bằng ms. Biến static bắt đầu bằng 0/false.
 * double-click: từ lần THẢ ổn định thứ nhất tới lần NHẤN ổn định thứ hai.
 * Nếu lần nhấn thứ hai trở thành nhấn giữ: hủy click đang chờ, ưu tiên giữ.
 */
typedef struct {
    bool raw_pressed;
    bool stable_pressed;
    uint64_t raw_changed_ms;
    uint64_t pressed_ms;
    bool waiting_second;
    bool second_press;
    uint64_t released_ms;
    bool long_mode;
    uint64_t last_repeat_ms;
} button_state_t;

static button_state_t button;

static void button_update(bool raw_pressed, uint64_t now)
{
    /* Mỗi lần tín hiệu thô đổi mức, bắt đầu lại thời gian chống dội.
     * Áp dụng cho cả nhấn và thả.
     */
    if (raw_pressed != button.raw_pressed) {
        button.raw_pressed = raw_pressed;
        button.raw_changed_ms = now;
    }

    if (button.stable_pressed != button.raw_pressed &&
        now - button.raw_changed_ms >= DEBOUNCE_MS) {
        button.stable_pressed = button.raw_pressed;
        if (button.stable_pressed) {
            button.second_press = button.waiting_second &&
                now - button.released_ms <= DOUBLE_CLICK_MS;
            /* Nhấn lần mới sau cửa sổ: hoàn thành click cũ trước. */
            if (button.waiting_second && !button.second_press) {
                counter_up();
            }
            button.waiting_second = false;
            button.pressed_ms = now;
            button.long_mode = false;
        } else {
            if (button.long_mode) {
                /* Thả sau giữ: không tạo click, không tăng thêm. */
                button.long_mode = false;
                button.waiting_second = false;
            } else if (button.second_press) {
                counter_down();
                button.waiting_second = false;
            } else {
                button.waiting_second = true;
                button.released_ms = now;
            }
            button.second_press = false;
        }
    }

    /* Khi tín hiệu thô báo thả, tạm ngừng tăng ngay, dù còn đang debounce.
     * Chỉ kết thúc trạng thái nhấn khi lần thả được công nhận ổn định.
     */
     if (button.stable_pressed && button.raw_pressed) {
        if (!button.long_mode && now - button.pressed_ms >= LONG_PRESS_MS) {
            button.long_mode = true;
            button.second_press = false;
            button.waiting_second = false;
            button.last_repeat_ms = now;
            counter_up();
        } else if (button.long_mode && now - button.last_repeat_ms >= REPEAT_MS) {
            button.last_repeat_ms = now;
            counter_up();
        }
    }

    /* Chờ hết cửa sổ mới công nhận single click, tránh double bị +1 rồi -1. */
    if (!button.stable_pressed && button.waiting_second &&
        now - button.released_ms > DOUBLE_CLICK_MS) {
        button.waiting_second = false;
        counter_up();
    }
}

void app_main(void)
{
    hardware_init();
    while (1) {
        uint64_t now = (uint64_t)esp_timer_get_time() / MICROSECONDS_PER_MS;
        bool pressed = (REG32(INPUT_ADDR) & BIT32(BUTTON_PIN)) == 0;
        button_update(pressed, now);
        /* Một tick để nhường CPU. Không delay 350/500/800ms vì sẽ bỏ lỡ nút.
         * Độ phân giải polling phụ thuộc CONFIG_FREERTOS_HZ (100Hz -> 10ms).
         */
        vTaskDelay(1);
    }
}