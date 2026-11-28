#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdbool.h>

#define DR_REG_GPIO_BASE      (0x60004000UL)
#define DR_REG_IO_MUX_BASE    (0x60009000UL)
#define GPIO_ENABLE_W1TS_REG  (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x24U))
#define GPIO_ENABLE1_W1TS_REG (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x30U))
#define GPIO_ENABLE_W1TC_REG  (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x28U))
#define GPIO_ENABLE1_W1TC_REG (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x34U))
#define GPIO_OUT_W1TS_REG     (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x08U))
#define GPIO_OUT1_W1TS_REG    (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x14U))
#define GPIO_OUT_W1TC_REG     (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x0CU))
#define GPIO_OUT1_W1TC_REG    (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x18U))
#define GPIO_IN_REG           (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x3CU))

#define IO_MUX_GPIO_REG(n) (*(volatile uint32_t*) (DR_REG_IO_MUX_BASE + 0x4U + (n) * 4U))

#define IO_MUX_FUN_WPU_POS (8U)
#define IO_MUX_FUN_IE_POS  (9U)
#define IO_MUX_FUN_DRV_POS (10U)
#define IO_MUX_MCU_SEL_POS (12U)

#define REGISTER_WIDTH_BITS (32U)
#define BTN_PIN             (14U)
uint8_t COUNT_NUM = 0;
uint32_t COUNT_TIME = 0;
bool LAST_BUT = false;
uint8_t COUNT_WINDOW_TIME = 0;
uint8_t FIRST_COUNT = 0;
#define TIME_COUNT_DB        (25U)
#define TIME_COUNT_DOWN      (350U)
#define TIME_COUNT_LONGPRESS (500U)

typedef enum
{
    SEG_A,
    SEG_B,
    SEG_C,
    SEG_D,
    SEG_E,
    SEG_F,
    SEG_G,
    SEG_COUNT
} segment_t;

// Chỉ số là tên đoạn, giá trị là số GPIO.
// Thay các số này theo cách bạn nối dây.
static const uint32_t segment_pins[SEG_COUNT] = {
    [SEG_A] = 4U,
    [SEG_B] = 5U,
    [SEG_C] = 6U,
    [SEG_D] = 7U,
    [SEG_E] = 15U,
    [SEG_F] = 16U,
    [SEG_G] = 17U,
};

// static const uint8_t digit_segments[10] = {
//     [0] = 0x3F, // A B C D E F
//     [1] = 0x06, //   B C
//     [2] = 0x5B, // A B   D E   G
//     [3] = 0x4F, // A B C D     G
//     [4] = 0x66, //   B C     F G
//     [5] = 0x6D, // A   C D   F G
//     [6] = 0x7D, // A   C D E F G
//     [7] = 0x07, // A B C
//     [8] = 0x7F, // A B C D E F G
//     [9] = 0x6F, // A B C D   F G
// };

static const uint8_t digit_segments[10][SEG_COUNT] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}, // 9
};

// Configure GPIO4 as input pullup
void gpio_init_sw(void)
{
    // IO_MUX_FUN_WPU = 1
    IO_MUX_GPIO_REG(BTN_PIN) |= (1UL << IO_MUX_FUN_WPU_POS);

    // IO_MUX_MCU_SEL = 0
    IO_MUX_GPIO_REG(BTN_PIN) &= ~(7UL << IO_MUX_MCU_SEL_POS);

    // Disable output
    GPIO_ENABLE_W1TC_REG |= (1UL << BTN_PIN);

    // IO_MUX_FUN_IE = 1
    IO_MUX_GPIO_REG(BTN_PIN) |= (1UL << IO_MUX_FUN_IE_POS);
}

void gpio_init_output(uint32_t pin)
{
    // Chọn chức năng GPIO: MCU_SEL = 1 trên ESP32-S3.
    IO_MUX_GPIO_REG(pin) &= ~(7UL << IO_MUX_MCU_SEL_POS);
    IO_MUX_GPIO_REG(pin) |= (1UL << IO_MUX_MCU_SEL_POS);

    // Cấu hình trường drive strength = 1.
    IO_MUX_GPIO_REG(pin) &= ~(3UL << IO_MUX_FUN_DRV_POS);
    IO_MUX_GPIO_REG(pin) |= (1UL << IO_MUX_FUN_DRV_POS);

    // Tắt chức năng input.
    IO_MUX_GPIO_REG(pin) &= ~(1UL << IO_MUX_FUN_IE_POS);

    // Bật output theo nhóm thanh ghi chứa chân đó.
    if (pin < REGISTER_WIDTH_BITS)
    {
        GPIO_ENABLE_W1TS_REG = (1UL << pin);
    }
    else
    {
        GPIO_ENABLE1_W1TS_REG = (1UL << (pin - REGISTER_WIDTH_BITS));
    }
}

// void gpio_init_led(void)
// {
//     for (uint32_t seg = 0U; seg < SEG_COUNT; seg++)
//     {
//         gpio_init_output(segment_pins[seg]);
//     }
// }

// Configure GPIO45 as output
void gpio_init_led(void)
{
    for (uint32_t seg = 0U; seg < SEG_COUNT; seg++)
    {
        uint32_t pin = segment_pins[seg];
        // Select the GPIO function on ESP32-S3.
        IO_MUX_GPIO_REG(pin) &= ~(7UL << IO_MUX_MCU_SEL_POS);
        IO_MUX_GPIO_REG(pin) |= (1UL << IO_MUX_MCU_SEL_POS);
        // Set drive strength field to 1 and disable input.
        IO_MUX_GPIO_REG(pin) &= ~(3UL << IO_MUX_FUN_DRV_POS);
        IO_MUX_GPIO_REG(pin) |= (1UL << IO_MUX_FUN_DRV_POS);
        IO_MUX_GPIO_REG(pin) &= ~(1UL << IO_MUX_FUN_IE_POS);
        // W1TS registers use assignment: writing zero leaves other pins alone.
        if (pin < REGISTER_WIDTH_BITS)
        {
            GPIO_ENABLE_W1TS_REG |= (1UL << pin);
        }
        else
        {
            GPIO_ENABLE1_W1TS_REG |= (1UL << (pin - REGISTER_WIDTH_BITS));
        }
    }
}

void gpio_register_set_high(uint32_t pin)
{
    if (pin < REGISTER_WIDTH_BITS)
    {
        GPIO_OUT_W1TS_REG |= (1UL << pin);
    }
    else
    {
        GPIO_OUT1_W1TS_REG |= (1UL << (pin - REGISTER_WIDTH_BITS));
    }
}

void gpio_register_set_low(uint32_t pin)
{
    if (pin < REGISTER_WIDTH_BITS)
    {
        GPIO_OUT_W1TC_REG |= (1UL << pin);
    }
    else
    {
        GPIO_OUT1_W1TC_REG |= (1UL << (pin - REGISTER_WIDTH_BITS));
    }
}

uint32_t gpio_read(uint32_t pin)
{
    return ((GPIO_IN_REG & (1UL << pin)) != 0U);
}

void seven_segment_show(uint8_t digit)
{
    if (digit >= 10U)
    {
        return;
    }

    for (uint32_t seg = 0; seg < SEG_COUNT; seg++)
    {
        bool on = digit_segments[digit][seg] != 0U;
        // bool high = COMMON_ANODE ? !on : on;

        uint32_t pin = segment_pins[seg];
        bool high = on;
        gpio_register_set_high(pin);
        if (high)
        {
            gpio_register_set_high(pin);
        }
        else
        {
            gpio_register_set_low(pin);
        }
    }
}

void program1(void)
{
    static bool ready = true;

    // Sau double click hoặc long press, phải thả mới nhận lượt mới.
    if (!ready)
    {
        if (gpio_read(BTN_PIN) != 0U)
        {
            vTaskDelay(pdMS_TO_TICKS(TIME_COUNT_DB));

            if (gpio_read(BTN_PIN) != 0U)
                ready = true;
        }
        return;
    }

    if (gpio_read(BTN_PIN) != 0U)
        return;

    // Xác nhận lần nhấn đầu.
    vTaskDelay(pdMS_TO_TICKS(TIME_COUNT_DB));

    if (gpio_read(BTN_PIN) != 0U)
        return;

    bool released = false;

    // 1. Kiểm tra giữ nút khoảng 500 ms.
    for (uint32_t i = 0U; i < TIME_COUNT_LONGPRESS; i++)
    {
        vTaskDelay(pdMS_TO_TICKS(1));

        if (gpio_read(BTN_PIN) != 0U)
        {
            vTaskDelay(pdMS_TO_TICKS(TIME_COUNT_DB));

            if (gpio_read(BTN_PIN) != 0U)
            {
                released = true;
                break;
            }
        }
    }

    if (!released)
    {
        COUNT_NUM = (COUNT_NUM + 1U) % 10U;
        seven_segment_show(COUNT_NUM);
        if (gpio_read(BTN_PIN) != 0U)
        {
            ready = false;
            return;
        }
        return;
    }

    // 2. Đã thả: chờ lần nhấn thứ hai khoảng 350 ms.
    for (uint32_t i = 0U; i < TIME_COUNT_DOWN; i++)
    {
        if (gpio_read(BTN_PIN) == 0U)
        {
            vTaskDelay(pdMS_TO_TICKS(TIME_COUNT_DB));

            if (gpio_read(BTN_PIN) == 0U)
            {
                COUNT_NUM = (COUNT_NUM + 9U) % 10U;
                seven_segment_show(COUNT_NUM);

                ready = false;
                return;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }

    // 3. Không có lần nhấn thứ hai: click đơn.
    COUNT_NUM = (COUNT_NUM + 1U) % 10U;
    seven_segment_show(COUNT_NUM);
}

void app_main(void)
{
    gpio_init_sw();
    gpio_init_led();

    while (1)
    {
        program1();
    }
}
