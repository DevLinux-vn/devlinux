/*
 * ESP32-S3 DevKitC-1 board revision: v1.0 (verify the silkscreen before use).
 * Onboard WS2812 LED GPIO: 48 (use GPIO 38 for board revision v1.1).
 * UART command console: UART0, TX GPIO43, RX GPIO44, 115200-8-N-1.
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "led_strip.h"

#define UART_PORT             UART_NUM_0
#define UART_TX_GPIO          GPIO_NUM_43
#define UART_RX_GPIO          GPIO_NUM_44
#define UART_BAUD_RATE        115200
#define UART_RX_BUFFER_SIZE   1024
#define UART_TX_BUFFER_SIZE   1024
#define UART_EVENT_QUEUE_SIZE 10
#define UART_READ_TIMEOUT     pdMS_TO_TICKS(20)
#define UART_TASK_STACK_SIZE  4096
#define UART_TASK_PRIORITY    12
#define UART_TASK_CORE        0

#define LED_STRIP_GPIO        GPIO_NUM_48 /* Change to GPIO_NUM_38 for DevKitC-1 v1.1. */
#define LED_STRIP_PIXEL_COUNT 1
#define LED_BRIGHTNESS        16

#define COMMAND_BUFFER_SIZE 64
#define COMMAND_LED_ON      "LED_ON"
#define COMMAND_LED_OFF     "LED_OFF"
#define COMMAND_RED         "RED"
#define COMMAND_GREEN       "GREEN"
#define COMMAND_BLUE        "BLUE"

static const char *TAG = "uart_console";
static QueueHandle_t uart_event_queue;
static led_strip_handle_t led_strip;

static void set_led(uint8_t red, uint8_t green, uint8_t blue)
{
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, red, green, blue));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

static void initialize_led(void)
{
    const led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO,
        .max_leds = LED_STRIP_PIXEL_COUNT,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };
    const led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .flags.with_dma = false,
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_ERROR_CHECK(led_strip_clear(led_strip));
}

static void initialize_uart(void)
{
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_GPIO, UART_RX_GPIO,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_RX_BUFFER_SIZE,
                                        UART_TX_BUFFER_SIZE, UART_EVENT_QUEUE_SIZE,
                                        &uart_event_queue, 0));
}

static void process_command(const char *command)
{
    ESP_LOGI(TAG, "Received command: \"%s\"", command);

    if (strcmp(command, COMMAND_LED_ON) == 0) {
        set_led(LED_BRIGHTNESS, LED_BRIGHTNESS, LED_BRIGHTNESS);
        ESP_LOGI(TAG, "LED -> WHITE");
    } else if (strcmp(command, COMMAND_LED_OFF) == 0) {
        ESP_ERROR_CHECK(led_strip_clear(led_strip));
        ESP_LOGI(TAG, "LED -> OFF");
    } else if (strcmp(command, COMMAND_RED) == 0) {
        set_led(LED_BRIGHTNESS, 0, 0);
        ESP_LOGI(TAG, "LED -> RED");
    } else if (strcmp(command, COMMAND_GREEN) == 0) {
        set_led(0, LED_BRIGHTNESS, 0);
        ESP_LOGI(TAG, "LED -> GREEN");
    } else if (strcmp(command, COMMAND_BLUE) == 0) {
        set_led(0, 0, LED_BRIGHTNESS);
        ESP_LOGI(TAG, "LED -> BLUE");
    } else {
        ESP_LOGW(TAG, "Unknown command: \"%s\"; LED state unchanged", command);
    }
}

static void submit_line(char *command, size_t *length, bool *too_long)
{
    if (*too_long) {
        ESP_LOGW(TAG, "Command rejected: line exceeds %u characters",
                 (unsigned int)(COMMAND_BUFFER_SIZE - 1));
    } else if (*length > 0) {
        command[*length] = '\0';
        process_command(command);
    }

    *length = 0;
    *too_long = false;
}

static void handle_received_byte(char byte, char *command, size_t *length, bool *too_long)
{
    if (byte == '\r' || byte == '\n') {
        static const char newline[] = "\r\n";
        uart_write_bytes(UART_PORT, newline, sizeof(newline) - 1);
        submit_line(command, length, too_long);
    } else if (byte == '\b' || (uint8_t)byte == 0x7f) {
        if (*length > 0 && !*too_long) {
            --(*length);
            static const char erase[] = "\b \b";
            uart_write_bytes(UART_PORT, erase, sizeof(erase) - 1);
        }
    } else {
        if (!*too_long && *length < COMMAND_BUFFER_SIZE - 1) {
            uart_write_bytes(UART_PORT, &byte, 1);
            command[(*length)++] = byte;
        } else {
            *too_long = true;
            uart_write_bytes(UART_PORT, &byte, 1);
        }
    }
}

static void reset_uart_input(char *command, size_t *length, bool *too_long)
{
    ESP_ERROR_CHECK(uart_flush_input(UART_PORT));
    xQueueReset(uart_event_queue);
    command[0] = '\0';
    *length = 0;
    *too_long = false;
}

static void uart_event_task(void *parameter)
{
    (void)parameter;
    uart_event_t event;
    uint8_t data[UART_RX_BUFFER_SIZE];
    char command[COMMAND_BUFFER_SIZE] = {0};
    size_t command_length = 0;
    bool line_too_long = false;

    ESP_LOGI(TAG, "UART0 command console ready");
    while (true) {
        if (xQueueReceive(uart_event_queue, &event, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        switch (event.type) {
        case UART_DATA: {
            size_t remaining = event.size;
            while (remaining > 0) {
                const size_t chunk_size = remaining < sizeof(data) ? remaining : sizeof(data);
                const int bytes_read = uart_read_bytes(UART_PORT, data, chunk_size, UART_READ_TIMEOUT);
                if (bytes_read <= 0) {
                    break;
                }
                for (int index = 0; index < bytes_read; ++index) {
                    handle_received_byte((char)data[index], command, &command_length, &line_too_long);
                }
                remaining -= (size_t)bytes_read;
            }
            break;
        }
        case UART_FIFO_OVF:
            ESP_LOGW(TAG, "UART FIFO overflow; input and command line reset");
            reset_uart_input(command, &command_length, &line_too_long);
            break;
        case UART_BUFFER_FULL:
            ESP_LOGW(TAG, "UART RX buffer full; input and command line reset");
            reset_uart_input(command, &command_length, &line_too_long);
            break;
        case UART_PARITY_ERR:
            ESP_LOGW(TAG, "UART parity error");
            break;
        case UART_FRAME_ERR:
            ESP_LOGW(TAG, "UART frame error");
            break;
        default:
            ESP_LOGD(TAG, "UART event type: %d", event.type);
            break;
        }
    }
}

void app_main(void)
{
    initialize_led();
    initialize_uart();

    const BaseType_t task_created = xTaskCreatePinnedToCore(
        uart_event_task, "uart_event_task", UART_TASK_STACK_SIZE,
        NULL, UART_TASK_PRIORITY, NULL, UART_TASK_CORE);
    ESP_ERROR_CHECK(task_created == pdPASS ? ESP_OK : ESP_FAIL);
}
