/* DevKitC-1 v1.1, RGB LED on GPIO38 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"

#define UART_PORT_NUM      UART_NUM_0
#define UART_TX_PIN        GPIO_NUM_43
#define UART_RX_PIN        GPIO_NUM_44
#define UART_BAUD_RATE     115200UL
#define UART_BUF_SIZE      1024U
#define UART_QUEUE_SIZE    10U
#define CMD_BUF_SIZE       64U

#define RGB_LED_PIN        GPIO_NUM_38 /* v1.0 boards: GPIO_NUM_48 */

#define CMD_LED_ON         "LED_ON"
#define CMD_LED_OFF        "LED_OFF"
#define CMD_RED            "RED"
#define CMD_GREEN          "GREEN"
#define CMD_BLUE           "BLUE"

static const char *TAG = "UART_CONSOLE";
static QueueHandle_t uart_queue;
static led_strip_handle_t led_strip;

static void handle_command(const char *cmd) {
    ESP_LOGI(TAG, "Received command: \"%s\"", cmd);
    if (strcmp(cmd, CMD_LED_ON) == 0) {
        led_strip_set_pixel(led_strip, 0, 255, 255, 255);
        led_strip_refresh(led_strip);
        ESP_LOGI(TAG, "LED -> WHITE");
    } else if (strcmp(cmd, CMD_LED_OFF) == 0) {
        led_strip_clear(led_strip);
        ESP_LOGI(TAG, "LED -> OFF");
    } else if (strcmp(cmd, CMD_RED) == 0) {
        led_strip_set_pixel(led_strip, 0, 255, 0, 0);
        led_strip_refresh(led_strip);
        ESP_LOGI(TAG, "LED -> RED");
    } else if (strcmp(cmd, CMD_GREEN) == 0) {
        led_strip_set_pixel(led_strip, 0, 0, 255, 0);
        led_strip_refresh(led_strip);
        ESP_LOGI(TAG, "LED -> GREEN");
    } else if (strcmp(cmd, CMD_BLUE) == 0) {
        led_strip_set_pixel(led_strip, 0, 0, 0, 255);
        led_strip_refresh(led_strip);
        ESP_LOGI(TAG, "LED -> BLUE");
    } else {
        ESP_LOGW(TAG, "Unknown command: \"%s\"", cmd);
    }
}

static void uart_event_task(void *pvParameters) {
    uart_event_t event;
    uint8_t dtmp[CMD_BUF_SIZE];
    char cmd_buf[CMD_BUF_SIZE];
    int cmd_len = 0;

    ESP_LOGI(TAG, "Console ready on UART0, 115200-8-N-1");

    for (;;) {
        if (xQueueReceive(uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            switch (event.type) {
                case UART_DATA:
                    uart_read_bytes(UART_PORT_NUM, dtmp, event.size, portMAX_DELAY);
                    for (int i = 0; i < event.size; i++) {
                        char c = (char)dtmp[i];
                        if (c == '\r' || c == '\n') {
                            if (cmd_len > 0) {
                                cmd_buf[cmd_len] = '\0';
                                const char rn[] = "\r\n";
                                uart_write_bytes(UART_PORT_NUM, rn, 2);
                                handle_command(cmd_buf);
                                cmd_len = 0;
                            }
                        } else if (c == '\b' || c == 127) { // Backspace or DEL
                            if (cmd_len > 0) {
                                cmd_len--;
                                const char bs[] = "\b \b";
                                uart_write_bytes(UART_PORT_NUM, bs, 3);
                            }
                        } else if (c >= 32 && c < 127) { // Printable characters only
                            if (cmd_len < CMD_BUF_SIZE - 1) {
                                cmd_buf[cmd_len++] = c;
                                uart_write_bytes(UART_PORT_NUM, &c, 1);
                            }
                        }
                    }
                    break;
                case UART_FIFO_OVF:
                case UART_BUFFER_FULL:
                    uart_flush_input(UART_PORT_NUM);
                    xQueueReset(uart_queue);
                    cmd_len = 0;
                    ESP_LOGW(TAG, "UART overflow, input flushed");
                    break;
                default:
                    ESP_LOGI(TAG, "UART event type: %d ignored", event.type);
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}

void app_main(void) {
    /* 1. Initialize the RGB LED */
    led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_LED_PIN,
        .max_leds = 1, 
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, 
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    led_strip_clear(led_strip);

    /* 2. Configure UART */
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE, UART_BUF_SIZE, UART_QUEUE_SIZE, &uart_queue, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    /* 3. Start Event Task */
    xTaskCreatePinnedToCore(uart_event_task, "uart_event_task", 4096, NULL, 12, NULL, 0);
}