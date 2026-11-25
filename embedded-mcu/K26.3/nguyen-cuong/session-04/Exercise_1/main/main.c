/*
 * ESP32-S3 / ESP-IDF v5.2 — LED 7 đoạn ANODE CHUNG, bộ đếm dùng ngắt GPIO.
 * COM -> 3V3. a,b,c,d,e,f,g -> GPIO4,5,6,7,15,16,17.
 * Mỗi đoạn có điện trở riêng 220–330 ohm (mạch hiện tại: 300 ohm).
 * Nút: GPIO14 -> GND, pull-up nội. DP không nối. Mạch giữ nguyên Session 03.
 *
 * Phân chia:
 *  - LED: chép nguyên code thanh ghi từ Session 03 (IO_MUX, GPIO matrix,
 *    GPIO_ENABLE, W1TS/W1TC). Không dùng driver cho LED.
 *  - Nút: chỉ dùng driver/gpio.h. ISR ANYEDGE chỉ đọc mức chân, lấy timestamp,
 *    lưu trạng thái. Chống dội và giải mã cử chỉ nằm trong app_main().
 *  - Chân nút CHỈ được đọc trong ISR: bỏ ngắt thì không còn phát hiện nút.
 */
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

/* ======================= LED: code thanh ghi từ Session 03 ================= */

/* Địa chỉ nền lấy từ memory map ESP32-S3 / reg_base.h của IDF v5.2. */
#define GPIO_BASE                  0x60004000UL
#define IO_MUX_BASE                0x60009000UL
/* * truy cập nội dung; volatile giữ các lần đọc/ghi phần cứng; uint32_t: 32 bit. */
#define REG32(address)             (*(volatile uint32_t *)(uintptr_t)(address))
/* Session 03 đặt tên BIT32(n); đổi thành SBIT32(n) vì driver/gpio.h kéo theo
 * esp_bit_defs.h, nơi đã định nghĩa sẵn BIT32 (bit 32 của số 64 bit).
 */
#define SBIT32(n)                  (1UL << (n))
#define IO_MUX_ADDR(n)             (IO_MUX_BASE + 0x04UL + 4UL * (n))
#define GPIO_PIN_ADDR(n)           (GPIO_BASE + 0x74UL + 4UL * (n))
#define GPIO_ROUTE_ADDR(n)         (GPIO_BASE + 0x554UL + 4UL * (n))
#define OUT_SET_ADDR               (GPIO_BASE + 0x08UL)
#define OUT_CLEAR_ADDR             (GPIO_BASE + 0x0CUL)
#define ENABLE_SET_ADDR            (GPIO_BASE + 0x24UL)
#define ENABLE_CLEAR_ADDR          (GPIO_BASE + 0x28UL)

#define MUX_FUNCTION_SHIFT         12U
#define MUX_FUNCTION_MASK          (7UL << MUX_FUNCTION_SHIFT)
#define MUX_GPIO_FUNCTION          1UL
#define MUX_DRIVE_SHIFT            10U
#define MUX_DRIVE_MASK             (3UL << MUX_DRIVE_SHIFT)
#define MUX_INPUT_ENABLE           SBIT32(9)
#define MUX_PULLUP                 SBIT32(8)
#define MUX_PULLDOWN               SBIT32(7)
#define PIN_OPEN_DRAIN             SBIT32(2)
#define ROUTE_SIGNAL_MASK          0x1FFUL
#define ROUTE_GPIO_SIGNAL          256UL
#define ROUTE_INVERT               SBIT32(9)
#define ROUTE_SOFTWARE_ENABLE      SBIT32(10)
#define ROUTE_ENABLE_INVERT        SBIT32(11)

#define SEGMENT_COUNT              7U
#define DISPLAY_COMMON_ANODE       1
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
        if ((digit_table[digit] & SBIT32(i)) != 0) {
            lit_pins |= SBIT32(segment_pins[i]);
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

/* Giống hardware_init() của Session 03 nhưng bỏ phần cấu hình nút bằng
 * thanh ghi: GPIO14 giờ do driver quản lý (button_init()).
 */
static void display_init(void)
{
    for (uint32_t i = 0; i < SEGMENT_COUNT; ++i) {
        segment_mask |= SBIT32(segment_pins[i]);
    }
    /* Tạm tắt đầu ra trong lúc chuẩn bị, tránh xuất nhầm mức. */
    REG32(ENABLE_CLEAR_ADDR) = segment_mask;

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

    counter = 0;
    display_digit(counter);                 /* Chuẩn bị số 0 trước. */
    REG32(ENABLE_SET_ADDR) = segment_mask;   /* Sau đó mới bật đầu ra. */
}

/* ======================= Bộ đếm ========================================== */

static const char *TAG = "counter";

/* Log tùy chọn, chỉ gọi từ app_main(), không bao giờ từ ISR. */
static void counter_up(const char *gesture)
{
    counter = (uint8_t)((counter + 1U) % DIGIT_COUNT);
    display_digit(counter);
    ESP_LOGI(TAG, "%s: +1 -> %u", gesture, (unsigned)counter);
}

static void counter_down(const char *gesture)
{
    /* +9 tương đương -1 theo modulo 10, tránh kết quả âm khi đang ở 0. */
    counter = (uint8_t)((counter + DIGIT_COUNT - 1U) % DIGIT_COUNT);
    display_digit(counter);
    ESP_LOGI(TAG, "%s: -1 -> %u", gesture, (unsigned)counter);
}

/* ======================= Nút: ISR + trạng thái chia sẻ ===================== */

#define BUTTON_GPIO                GPIO_NUM_14
#define BUTTON_PRESSED_LEVEL       0      /* pull-up: nhấn kéo xuống GND */
#define LOOP_PERIOD_MS             10U

/* Ngưỡng thời gian giữ như Session 03, đổi sang µs cho esp_timer_get_time(). */
#define US_PER_MS                  1000LL
#define DEBOUNCE_US                (25LL * US_PER_MS)
#define DOUBLE_CLICK_US            (350LL * US_PER_MS)
#define LONG_PRESS_US              (800LL * US_PER_MS)
#define REPEAT_US                  (500LL * US_PER_MS)

/* Dữ liệu ISR ghi, app_main đọc. volatile: buộc compiler đọc lại RAM mỗi lần.
 * int64_t trên Xtensa 32 bit cần 2 lệnh truy cập, và 3 biến phải khớp nhau,
 * nên mọi truy cập đều nằm trong critical section dùng chung spinlock.
 */
static volatile bool    isr_pressed;      /* mức đọc ở cạnh gần nhất */
static volatile int64_t isr_press_us;     /* thời điểm cạnh 1 -> 0 gần nhất */
static volatile int64_t isr_release_us;   /* thời điểm cạnh 0 -> 1 gần nhất */
static portMUX_TYPE button_lock = portMUX_INITIALIZER_UNLOCKED;

/* ISR chỉ: đọc mức chân, lấy timestamp, lưu trạng thái.
 * Không log, không delay, không giải mã cử chỉ, không ghi LED.
 * esp_timer_get_time() nằm trong IRAM. Service cài với cờ 0 (không có
 * ESP_INTR_FLAG_IRAM) nên ISR không chạy khi cache flash bị tắt; vì vậy gọi
 * gpio_get_level() ở đây an toàn.
 */
static void IRAM_ATTR button_isr(void *arg)
{
    (void)arg;
    int64_t now_us = esp_timer_get_time();
    bool pressed = gpio_get_level(BUTTON_GPIO) == BUTTON_PRESSED_LEVEL;

    portENTER_CRITICAL_ISR(&button_lock);
    isr_pressed = pressed;
    if (pressed) {
        isr_press_us = now_us;
    } else {
        isr_release_us = now_us;
    }
    portEXIT_CRITICAL_ISR(&button_lock);
}

static void button_init(void)
{
    const gpio_config_t config = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,   /* bắt cả nhấn lẫn thả */
    };
    ESP_ERROR_CHECK(gpio_config(&config));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_GPIO, button_isr, NULL));
}

/* Bản sao nhất quán của 3 biến chia sẻ, lấy một lần mỗi vòng lặp. */
typedef struct {
    bool pressed;
    int64_t press_us;
    int64_t release_us;
} button_snapshot_t;

static button_snapshot_t button_snapshot(void)
{
    button_snapshot_t snap;
    portENTER_CRITICAL(&button_lock);
    snap.pressed = isr_pressed;
    snap.press_us = isr_press_us;
    snap.release_us = isr_release_us;
    portEXIT_CRITICAL(&button_lock);
    return snap;
}

/* ======================= Giải mã cử chỉ (chạy trong app_main) ============== */

/* Mọi mốc thời gian là timestamp µs của cạnh do ISR lưu, không phải thời điểm
 * vòng lặp nhìn thấy, nên độ trễ 10 ms của vòng lặp không làm lệch phép đo.
 * double-click: từ lần THẢ ổn định thứ nhất tới lần NHẤN ổn định thứ hai.
 * Nếu lần nhấn thứ hai trở thành nhấn giữ: hủy click đang chờ, ưu tiên giữ.
 */
typedef struct {
    bool stable_pressed;      /* trạng thái đã chống dội */
    int64_t pressed_us;       /* cạnh nhấn của lần nhấn được công nhận */
    int64_t released_us;      /* cạnh thả của lần thả được công nhận */
    bool waiting_second;      /* đã có 1 click, chờ hết cửa sổ double */
    bool second_press;        /* lần nhấn hiện tại là nhấn thứ hai */
    bool long_mode;
    int64_t next_repeat_us;
} gesture_state_t;

static gesture_state_t gesture;

static void on_stable_press(int64_t edge_us)
{
    gesture.second_press = gesture.waiting_second &&
        edge_us - gesture.released_us <= DOUBLE_CLICK_US;
    /* Nhấn mới ngoài cửa sổ: hoàn thành click cũ trước. */
    if (gesture.waiting_second && !gesture.second_press) {
        counter_up("single click");
    }
    gesture.waiting_second = false;
    gesture.pressed_us = edge_us;
    gesture.long_mode = false;
}

static void on_stable_release(int64_t edge_us)
{
    if (gesture.long_mode) {
        /* Thả sau giữ: không tạo click, không tăng thêm. */
        gesture.long_mode = false;
        gesture.waiting_second = false;
    } else if (gesture.second_press) {
        counter_down("double click");
        gesture.waiting_second = false;
    } else {
        /* Chưa đếm: chờ xem có lần nhấn thứ hai không. */
        gesture.waiting_second = true;
        gesture.released_us = edge_us;
    }
    gesture.second_press = false;
}

static void gesture_update(const button_snapshot_t *snap, int64_t now_us)
{
    /* 1) Chống dội trên các cạnh đã lưu: chỉ công nhận mức mới khi từ cạnh
     *    cuối cùng (nhấn hoặc thả) đã yên đủ DEBOUNCE_US. Mỗi cạnh dội làm
     *    timestamp mới hơn, tự động bắt đầu lại thời gian chờ.
     */
    int64_t last_edge_us = snap->press_us > snap->release_us ?
                           snap->press_us : snap->release_us;
    if (snap->pressed != gesture.stable_pressed &&
        now_us - last_edge_us >= DEBOUNCE_US) {
        gesture.stable_pressed = snap->pressed;
        if (gesture.stable_pressed) {
            on_stable_press(snap->press_us);
        } else {
            on_stable_release(snap->release_us);
        }
    }

    /* 2) Nhấn giữ: so thời gian, không cần cạnh mới. Khi ISR báo đã thả thì
     *    ngừng lặp ngay, dù lần thả còn đang chống dội.
     */
    if (gesture.stable_pressed && snap->pressed) {
        if (!gesture.long_mode &&
            now_us - gesture.pressed_us >= LONG_PRESS_US) {
            gesture.long_mode = true;
            gesture.second_press = false;
            gesture.waiting_second = false;
            gesture.next_repeat_us = now_us + REPEAT_US;
            counter_up("long press");
        } else if (gesture.long_mode && now_us >= gesture.next_repeat_us) {
            gesture.next_repeat_us += REPEAT_US;   /* mốc cố định, không trôi */
            if (gesture.next_repeat_us <= now_us) {
                gesture.next_repeat_us = now_us + REPEAT_US; /* vòng lặp bị trễ */
            }
            counter_up("long repeat");
        }
    }

    /* 3) Hết cửa sổ double-click mới công nhận single click.
     *    Nếu ISR đã ghi một lần nhấn sau lần thả mà bước 1 chưa kết luận
     *    (chân đang ở mức nhấn, hoặc cạnh cuối chưa yên đủ DEBOUNCE_US),
     *    hoãn lại để bước 1 quyết định nó là nhấn thứ hai hay chỉ là nhiễu.
     */
    bool press_pending = snap->press_us > gesture.released_us &&
                         (snap->pressed ||
                          now_us - last_edge_us < DEBOUNCE_US);
    if (!gesture.stable_pressed && gesture.waiting_second && !press_pending &&
        now_us - gesture.released_us > DOUBLE_CLICK_US) {
        gesture.waiting_second = false;
        counter_up("single click");
    }
}

void app_main(void)
{
    display_init();
    button_init();

    while (1) {
        button_snapshot_t snap = button_snapshot();
        int64_t now_us = esp_timer_get_time();
        gesture_update(&snap, now_us);
        /* Không chờ 350/500/800 ms ở đây: mọi hạn chót đều so thời gian.
         * Với CONFIG_FREERTOS_HZ=100, 10 ms = 1 tick.
         */
        vTaskDelay(pdMS_TO_TICKS(LOOP_PERIOD_MS));
    }
}
