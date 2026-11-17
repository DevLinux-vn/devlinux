// Mục tiêu: Điều khiển LED 7 đoạn (Common Anode)
// Dùng code lập trình thanh ghi (bare-metal/register level) để hiện thực

#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h" //Cần để dùng hàm vTaskDelay của freeRTOS
#include "freertos/task.h"
#include "soc/gpio_num.h"
// #include "driver/gpio.h"
#include "esp_log.h" //Cần khi hàm ESP_LOGI

#define GPIO_BASE_ADDR 0x60004000UL
#define IO_MUX_BASE_ADDR 0x60009000UL

// GPIO_OUT_REG control register
#define GPIO_OUT_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x0004)
#define GPIO_OUT_W1TS_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x0008)
#define GPIO_OUT_W1TC_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x000C)

// GPIO_OUT1_REG control register
#define GPIO_OUT1_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x0010)
#define GPIO_OUT1_W1TS_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x0014)
#define GPIO_OUT1_W1TC_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x0018)

// GPIO_ENABLE_REG control register
#define GPIO_ENABLE_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x0020)
#define GPIO_ENABLE_W1TS_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x0024)
#define GPIO_ENABLE_W1TC_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x0028)

// GPIO_ENABLE1_REG
#define GPIO_ENABLE1_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x002C)
#define GPIO_ENABLE1_W1TS_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x0030)
#define GPIO_ENABLE1_W1TC_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x0034)

// GPIO_IN_REG
#define GPIO_IN_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x003C)
#define GPIO_IN1_REG *(volatile uint32_t *)(GPIO_BASE_ADDR + 0x0040)

// function-like macro for IO_MUX_REG
#define IO_MUX_GPIO_REG(n) (*(volatile uint32_t *)(IO_MUX_BASE_ADDR + 0x0004 + 4 * (n)))

// Bit positions in IO_MUX_GPIOx_REG
#define IO_MUX_FUN_IE_POS (9U)   // Input enable of the pin. 1: Input enabled; 0: Input disabled
#define IO_MUX_FUN_WPU_POS (8U)  // Pull-up enable of the pin. 1: Internal pull-up enabled; 0: Internal pull-up disabled
#define IO_MUX_FUN_DRV_POS (10U) // Drive strength of the pin. 0: 5mA; 1: 10mA; 2: 20mA; 3: 40mA
#define IO_MUX_MCU_SEL_POS (12U) // Select the function of the pin. 0: function 0; 1: function 1

// Constants
#define BUTTON_PIN GPIO_NUM_14
#define REGISTER_WIDTH_BIT 32U
#define BLINK_DELAY_MS 1000U
#define DEBOUNCE_DELAY_MS 25U
#define LONG_PRESS_THRESHOLD_MS 800U
#define LONG_PRESS_INCREMENT_MS 500U
#define DOUBLE_CLICK_THRESHOLD_MS 350U

// Value for 7 segment display (Common Anode) to show digits 0-9. O: LED segment on; 1: LED segment off
const uint8_t segment_map[10] = {
    0b11000000, // 0. (Common Anode: A,B,C,D,E,F on, G off)
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000  // 9

};
// Pin numbers that connect to the segments values in segment_map. The order is A,B,C,D,E,F,G,DP (Decimal Point)
const uint8_t segment_pins[8] = {
    GPIO_NUM_4,  // A
    GPIO_NUM_5,  // B
    GPIO_NUM_6,  // C
    GPIO_NUM_7,  // D
    GPIO_NUM_15, // E
    GPIO_NUM_16, // F
    GPIO_NUM_17, // G
    GPIO_NUM_18  // DP (Decimal Point)
};
// Configure pin as output
void gpio_init_output_pin(uint32_t pin)
{

    // Configure GPIO_FUNCx_OUT_SEL_CFG_REG
    // No need to config GPIO_FUNCx_OUT_SEL_CFG_REG because it default value is correct (256)

    // IO_MUX_MCU_SEL = 0 to select GPIO function 0 (normal GPIO function)
    IO_MUX_GPIO_REG(pin) &= ~(7UL << IO_MUX_MCU_SEL_POS); // clear 3 bits of IO_MUX_MCU_SEL

    // Config IO_MUX_GPIOx_REG (choose the drive through current 10mA)
    IO_MUX_GPIO_REG(pin) &= ~(3UL << IO_MUX_FUN_DRV_POS); // clear 2 bits of IO_MUX_FUN_DRV
    IO_MUX_GPIO_REG(pin) |= (1UL << IO_MUX_FUN_DRV_POS);  // set value IO_MUX_FUN_DRV to 10 mA (value 1)

    // Enable OUTPUT
    if (pin < REGISTER_WIDTH_BIT)
    {
        GPIO_ENABLE_W1TS_REG = (1UL << pin);
    }
    else
    {
        GPIO_ENABLE1_W1TS_REG = (1UL << (pin - REGISTER_WIDTH_BIT));
    }

    // IO_MUX_FUN_IE = 0 to disable input
    IO_MUX_GPIO_REG(pin) &= ~(1UL << IO_MUX_FUN_IE_POS); // Clear bit IO_MUX_FUN_IE
}

// Configure pin as input with pull-up
void gpio_init_input_pin(uint32_t pin)
{
    // IO_MUX_FUN_IE = 1 to enable input
    IO_MUX_GPIO_REG(pin) |= (1UL << IO_MUX_FUN_IE_POS); // Set bit IO_MUX_FUN_IE

    // IO_MUX_MCU_SEL = 0 to select GPIO function 0 (normal GPIO function)
    IO_MUX_GPIO_REG(pin) &= ~(7UL << IO_MUX_MCU_SEL_POS); // clear 3 bits of IO_MUX_MCU_SEL

    // disable output
    if (pin < REGISTER_WIDTH_BIT)
    {
        GPIO_ENABLE_W1TC_REG = (1UL << pin);
    }
    else
    {
        GPIO_ENABLE1_W1TC_REG = (1UL << (pin - REGISTER_WIDTH_BIT));
    }

    // IO_MUX_FUN_WPU_POS = 1 to enable pull-up
    IO_MUX_GPIO_REG(pin) |= (1UL << IO_MUX_FUN_WPU_POS);
}

uint32_t gpio_read(uint32_t pin)
{
    if (pin < REGISTER_WIDTH_BIT)
    {
        return (GPIO_IN_REG >> pin) & 0x1;
    }
    else
    {
        return (GPIO_IN1_REG >> (pin - REGISTER_WIDTH_BIT)) & 0x1;
    }
}

void gpio_out_set_high(uint32_t pin)
{
    if (pin < REGISTER_WIDTH_BIT)
    {
        GPIO_OUT_W1TS_REG = (1UL << pin);
    }
    else
    {
        GPIO_OUT1_W1TS_REG = (1UL << (pin - REGISTER_WIDTH_BIT));
    }
}

void gpio_out_set_low(uint32_t pin)
{
    if (pin < REGISTER_WIDTH_BIT)
    {
        GPIO_OUT_W1TC_REG = (1UL << pin);
    }
    else
    {
        GPIO_OUT1_W1TC_REG = (1UL << (pin - REGISTER_WIDTH_BIT));
    }
}

void led_7seg_display_number(uint8_t number)
{
    if (number > 9)
    {
        ESP_LOGE("LED_7SEG", "Invalid digit: %d. Must be 0-9.", number);
        return;
    }

    uint8_t segment_value = segment_map[number];

    for (int i = 0; i < 8; i++)
    {
        if ((segment_value >> i) & 0x1)
        {
            gpio_out_set_high(segment_pins[i]); // Turn off segment (Common Anode)
        }
        else
        {
            gpio_out_set_low(segment_pins[i]); // Turn on segment (Common Anode)
        }
    }
}

// Hàm kiểm tra nút bấm với cơ chế chống dội
uint8_t button_is_pressed(uint32_t pin)
{
    if (gpio_read(pin) == 0) // Phát hiện mức LOW (được bấm)
    {
        vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY_MS)); // Debounce delay
        if (gpio_read(pin) == 0)
        {
            return 1; // Xác nhận bấm thật
        }
    }
    return 0;
}

// Hàm chờ người dùng nhả nút ra (tránh bị kẹt/giữ phím)
void wait_for_button_release(uint32_t pin)
{
    while (gpio_read(pin) == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY_MS));
    }
}

uint8_t button_is_released(uint32_t pin)
{
    if (gpio_read(pin) == 1) // Phát hiện mức HIGH (được nhả)
    {
        vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY_MS)); // Debounce delay
        if (gpio_read(pin) == 1)
        {
            return 1; // Xác nhận nhả thật
        }
    }
    return 0;
}

void app_main(void)
{
    // Init GPIO pins for 7-segment display
    for (int i = 0; i < 8; i++)
    {
        gpio_init_output_pin(segment_pins[i]);
    }
    // Init GPIO pin for button input
    gpio_init_input_pin(BUTTON_PIN);

    uint8_t current_number = 0;
    // Display the initial number on the 7-segment display
    led_7seg_display_number(current_number);

    while (1)
    {
        if (button_is_pressed(BUTTON_PIN))
        {
            // Start checking for long-press window
            TickType_t start_time = xTaskGetTickCount();
            uint8_t long_press_detected = 0;

            while (gpio_read(BUTTON_PIN) == 0) // While button is still pressed
            {
                vTaskDelay(pdMS_TO_TICKS(10)); // Polling delay

                // Check if the button has been pressed for more than LONG_PRESS_THRESHOLD_MS
                if ((xTaskGetTickCount() - start_time) >= pdMS_TO_TICKS(LONG_PRESS_THRESHOLD_MS)) // Long press detected after 800ms
                {
                    long_press_detected = 1;
                    break; // Exit the loop if long press is detected
                }
            }

            if (long_press_detected)
            {
                start_time = xTaskGetTickCount();                                     // Reset start time
                while (gpio_read(BUTTON_PIN) == 0 || !button_is_released(BUTTON_PIN)) // While button is still pressed in long-press mode
                {
                    // Check if the button has been pressed for more than LONG_PRESS_INCREMENT_MS
                    if ((xTaskGetTickCount() - start_time) >= pdMS_TO_TICKS(LONG_PRESS_INCREMENT_MS)) // Increment every 500ms in long-press mode
                    {
                        current_number = (current_number + 1) % 10; // Increment and wrap around 0~9
                        led_7seg_display_number(current_number);    // Update display
                        start_time = xTaskGetTickCount();           // Reset start time for next increment
                    }
                    vTaskDelay(pdMS_TO_TICKS(10)); // Polling delay
                }
            }
            else
            {
                // Not in LONG-PRESS mode, button was released before 800ms
                // So we checking for SINGLE-CLICK or DOUBLE-CLICK
                start_time = xTaskGetTickCount(); // Reset start time
                while (xTaskGetTickCount() - start_time <= pdMS_TO_TICKS(DOUBLE_CLICK_THRESHOLD_MS))
                {
                    if (button_is_pressed(BUTTON_PIN))
                    {
                        // Detected a second press within 350ms, so it's a DOUBLE-CLICK
                        current_number = (current_number == 0) ? 9 : (current_number - 1); // Decrement and wrap around 0~9
                        led_7seg_display_number(current_number);                           // Update display
                        break;                                                             // Exit the loop after handling double-click
                    }
                    vTaskDelay(pdMS_TO_TICKS(10)); // Polling delay
                }

                if (xTaskGetTickCount() - start_time > pdMS_TO_TICKS(DOUBLE_CLICK_THRESHOLD_MS))
                {
                    // No second press detected within 350ms, so it's a SINGLE-CLICK
                    current_number = (current_number + 1) % 10; // Increment and wrap around 0~9
                    led_7seg_display_number(current_number);    // Update display
                }

                wait_for_button_release(BUTTON_PIN); // Wait for button release to avoid multiple detections
            }
        }
    }
}