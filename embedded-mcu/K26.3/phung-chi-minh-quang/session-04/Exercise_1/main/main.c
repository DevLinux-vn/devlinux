// The 7-sengment-LED Counter (Common Anode), Interrupt-Driven control button

#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h" //Cần để dùng hàm vTaskDelay của freeRTOS
#include "freertos/task.h"
#include "soc/gpio_num.h"
#include "driver/gpio.h"
#include "esp_log.h"   //Cần khi hàm ESP_LOGI
#include "esp_timer.h" //Cần để dùng hàm esp_timer_get_time() (đơn vị microseconds)

static const char *TAG = "GPIO_BASIC";
// Các biến volatile dùng chung giữa ISR và Main Loop
static volatile uint32_t s_btn_current_level = 1; // Mặc định pull-up là 1 (HIGH)
static volatile int64_t s_btn_edge_timestamp = 0; // Lưu timestamp (microseconds)
static volatile bool s_new_edge_flag = false;     // Cờ báo có ngắt mới

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

static void IRAM_ATTR button_isr_handler(void *arg)
{
    s_btn_current_level = (uint32_t)gpio_read(BUTTON_PIN); // Hoặc đọc trực tiếp từ thanh ghi GPIO_IN_REG
    s_btn_edge_timestamp = esp_timer_get_time();
    s_new_edge_flag = true;
}

typedef enum
{
    STATE_IDLE = 0,
    STATE_PRESSED,
    STATE_LONG_PRESSED,
    STATE_WAIT_DOUBLE_CLICK,
    STATE_WAIT_RELEASE,
} button_state_t;

void app_main(void)
{
    // Init GPIO pins for 7-segment display
    for (int i = 0; i < 8; i++)
    {
        gpio_init_output_pin(segment_pins[i]);
    }
    // Configure Button pin as Input with internal Pull-Up enabled, interrupt on falling edge
    // (Active-low button: logic level transitions 1 -> 0 when pressed)
    gpio_config_t button_config = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE, // Interrupt on any edge (both rising and falling edges)
    };
    gpio_config(&button_config);

    // Install the GPIO ISR service and attach our ISR handler to the button pin
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL));

    ESP_LOGI(TAG, "Waiting for button interrupt on GPIO%d...", BUTTON_PIN);

    button_state_t current_state = STATE_IDLE;
    uint8_t current_number = 0;
    led_7seg_display_number(current_number); // Display initial number (0)

    uint64_t time_start = 0;     // User for time marker
    uint64_t last_edge_time = 0; // User for button debounce check

    // Variable to save the stable button level after debounce.
    uint8_t stable_button_level = 1; // 1: Released (default), 0: Pressed

    while (1)
    {
        uint64_t current_time = esp_timer_get_time();

        //----NON-BLOCKING DEBOUNCE LOGIC----
        // Check if a new edge has been detected by the ISR
        if (s_new_edge_flag)
        {
            if (current_time - last_edge_time > DEBOUNCE_DELAY_MS * 1000ULL)
            {
                stable_button_level = s_btn_current_level;
                last_edge_time = current_time;
                s_new_edge_flag = false; // Reset the flag after processing
            }
            else
            {
                // Ignore this edge as it is happned in too short time (<25ms)
                s_new_edge_flag = false; // Reset the flag to avoid reprocessing
            }
        }

        //----STATE MACHINE LOGIC----
        // Handle gesture base on current_state
        switch (current_state)
        {
        case STATE_IDLE:
            if (stable_button_level == 0)
            { // Button pressed
                current_state = STATE_PRESSED;
                time_start = current_time; // Mark the time when button is pressed
            }
            break;

        case STATE_PRESSED:
            if (stable_button_level == 0)
            { // Button is still pressed
                if (current_time - time_start >= LONG_PRESS_THRESHOLD_MS * 1000ULL)
                {
                    current_state = STATE_LONG_PRESSED; // Hand over to long press state
                    ESP_LOGI(TAG, "Long Press Detected");
                    time_start = current_time; // Reset time_start for long press increment
                }
            }
            else
            {
                // Button released before long press threshold, consider it as a short press
                current_state = STATE_WAIT_DOUBLE_CLICK; // Hand over to wait for double click state
                ESP_LOGI(TAG, "Short Press Detected");
                time_start = current_time; // Mark the time when button is released
            }
            break;
        case STATE_LONG_PRESSED:
            if (stable_button_level == 0)
            { // Button is still pressed
                if (current_time - time_start >= LONG_PRESS_INCREMENT_MS * 1000ULL)
                {
                    // Increment the number
                    current_number = (current_number + 1) % 10;
                    led_7seg_display_number(current_number);
                    ESP_LOGI(TAG, "Long Press Increment: %d", current_number);
                    time_start = current_time; // Reset time_start for next increment
                }
            }
            else
            {
                // Button released after long press, go back to idle state
                current_state = STATE_IDLE;
                ESP_LOGI(TAG, "Button Released after Long Press");
            }
            break;
        case STATE_WAIT_DOUBLE_CLICK:
            if (stable_button_level == 0) // Button pressed again
            {
                // DOUBLE CLICK DETECTED (-1)
                current_number = (current_number == 0) ? 9 : current_number - 1; // Decrement the number
                led_7seg_display_number(current_number);
                ESP_LOGI(TAG, "Double Click Detected: %d", current_number);
                current_state = STATE_WAIT_RELEASE; // Go to wait for release state
            }
            else if(current_time - time_start >= DOUBLE_CLICK_THRESHOLD_MS * 1000ULL)
            {
                // Button is still released
                // Too late for double click, treat it as SINGLE CLICK
                // SINGLE CLICK CONFIRMED (+1)
                current_number = (current_number + 1) % 10; // Increment the number
                led_7seg_display_number(current_number);
                ESP_LOGI(TAG, "Single Click Confirmed: %d", current_number);
                current_state = STATE_IDLE; // Go back to idle state
            }
            break;
        case STATE_WAIT_RELEASE:
            if (stable_button_level == 1) // Button released
            {
                current_state = STATE_IDLE; // Go back to idle state
                ESP_LOGI(TAG, "Button Released");
            }
            break;
        default:
            current_state = STATE_IDLE; // Reset to idle state in case of unexpected state
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Small delay to avoid busy waiting
    }
}