#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "led_strip.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_check.h"
#include "string.h"

static const char *TAG = "UART_CONSOLE";

#define LED_GPIO_PIN 48
#define LED_NUM 1

static led_strip_handle_t led_strip_init_hardware(void)
{
    led_strip_config_t led_config = {
        .strip_gpio_num = LED_GPIO_PIN,
        .max_leds = LED_NUM,
        .led_model = LED_MODEL_WS2812,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz for LED WS2812
        .flags.with_dma = false,
    };

    led_strip_handle_t ret_strip = NULL;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_config, &rmt_config, &ret_strip));
    led_strip_clear(ret_strip);
    return ret_strip;
}

#define CONSOLE_UART_NUM UART_NUM_0
#define CONSOLE_TX_PIN GPIO_NUM_43
#define CONSOLE_RX_PIN GPIO_NUM_44
#define CONSOLE_BAUND_RATE 115200
#define UART_RX_BUF_SIZE 1024
#define UART_TX_BUF_SIZE 0
#define UART_EVENT_QUEUE_LEN 20

static QueueHandle_t s_uart_event_queue = NULL;

static esp_err_t uart_init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = CONSOLE_BAUND_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_RETURN_ON_ERROR(uart_param_config(CONSOLE_UART_NUM, &uart_config), TAG, "uart param config failed");
    ESP_RETURN_ON_ERROR(uart_set_pin(CONSOLE_UART_NUM, CONSOLE_TX_PIN, CONSOLE_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE), TAG, "uart set pin failed");
    ESP_RETURN_ON_ERROR(uart_driver_install(CONSOLE_UART_NUM, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE, UART_EVENT_QUEUE_LEN, &s_uart_event_queue, 0), TAG, "uart driver install failed");

    return ESP_OK;
}

#define UART_TASK_STACK_SIZE 4096
#define UART_TASK_PRIORITY 10
#define UART_READ_CHUNK_SIZE 128

// Echo back valid character, delete special character
// If meet ENTER --> return TRUE to confirm completed Command has been stored into cmd_buf array
// If meet other character --> return FALSE store into cmd_buf array
static bool console_echo_char(uint8_t ch, char *cmd_buf, int *cmd_pos, int max_len)
{
    if (cmd_buf == NULL || cmd_pos == NULL || max_len <= 1)
        return false;

    // Received BACKSPACE or DELETE character --> delete last character on the echo terminal
    if (ch == '\b' || ch == 0x7F)
    {
        if ((*cmd_pos) > 0)
        {
            (*cmd_pos)--;
            uart_write_bytes(CONSOLE_UART_NUM, "\b \b", 3);
        }
    }
    else if (ch == '\r' || ch == '\n')
    {
        if ((*cmd_pos) > 0)
        {
            cmd_buf[(*cmd_pos)] = '\0'; // write string end character, inform that the command is ready
            uart_write_bytes(CONSOLE_UART_NUM, "\r\n", 2);
            (*cmd_pos) = 0; // reset cmd_pos to 0, ready for next command
            return true;
        }
    }
    else if (ch >= 0x20 && ch < 0x7F)
    {
        if ((*cmd_pos) < max_len - 1)
        {
            cmd_buf[(*cmd_pos)] = (char)ch;
            (*cmd_pos)++;
            uart_write_bytes(CONSOLE_UART_NUM, (const char *)&ch, 1);
        }
    }

    return false;
}

static void LED_strip_cmd_handle(led_strip_handle_t led_strip, char *cmd)
{

    if (strcmp(cmd, "LED_ON") == 0)
    {
        led_strip_set_pixel(led_strip, 0, 255, 255, 255); // Bật trắng
        led_strip_refresh(led_strip);
    }
    else if (strcmp(cmd, "LED_OFF") == 0)
    {
        led_strip_clear(led_strip);
        led_strip_refresh(led_strip);
    }
    else if (strcmp(cmd, "RED") == 0)
    {
        led_strip_set_pixel(led_strip, 0, 255, 0, 0); // RED
        led_strip_refresh(led_strip);
    }
    else if(strcmp(cmd, "GREEN") == 0)
    {
        led_strip_set_pixel(led_strip, 0, 0, 255,0); // GREEN
        led_strip_refresh(led_strip);
    }
    else if(strcmp(cmd,"BLUE") == 0){
        led_strip_set_pixel(led_strip,0,0,0,255);//BLUE
        led_strip_refresh(led_strip);
    }
    else{
        ESP_LOGW(TAG, "Unknown command: %s", cmd);
    }

    return;
}

static void uart_task(void *pvParameters)
{
    led_strip_handle_t led_strip = (led_strip_handle_t)pvParameters; // Place holder for future input parameters, it could be a function pointer to handle led_strip

    uart_event_t event;
    uint8_t rx_buf[UART_READ_CHUNK_SIZE]; // Not decleare as "char rx_buf[]" because of choosing to keep data as raw byte
    char cmd_buf[UART_READ_CHUNK_SIZE];   // to store command from PC
    int cmd_pos = 0;                      // indexing to check each character in cmd_buf
    // Event loop driven by UART ISR
    while (1)
    {
        /*
         * Block infefinitely (portMAX_DELAY) until UART ISR post an event into uart_queue.
         * Consume 0% CPU while waiting for event
         */

        if (xQueueReceive(s_uart_event_queue, &event, portMAX_DELAY) == pdTRUE)
        {
            switch (event.type)
            {
            case UART_DATA:
                // UART data available
                // Read data from UART
                int len = uart_read_bytes(CONSOLE_UART_NUM, rx_buf, sizeof(rx_buf) - 1, pdMS_TO_TICKS(20));

                if (len > 0)
                {
                    rx_buf[len] = '\0'; // Add null terminator
                    ESP_LOGI(TAG, "[UART Interrupt RX] Event name: %s, Read: %d bytes -> %s", "UART_DATA", len, (char *)rx_buf);
                    // Loop through each character in rx_buf to delete special character ('\b',0x7F,'\r','\n') out of command
                    // Echo back character via UART
                    for (int i = 0; i < len; i++)
                    {
                        if (console_echo_char(rx_buf[i], cmd_buf, &cmd_pos, sizeof(cmd_buf)) == true)
                        {
                            ESP_LOGI(TAG, "Command: %s", cmd_buf);
                            // TODO: Gọi hàm điều khiển led_strip dựa trên cmd_buf tại đây
                            LED_strip_cmd_handle(led_strip, cmd_buf);
                        }
                    }
                }
                break;
            case UART_FIFO_OVF:
                // UART FIFO overflow
                ESP_LOGW(TAG, "HW FIFO overflow, flushing RX");
                uart_flush_input(CONSOLE_UART_NUM); // Clear both RX FIFO and RX Ring_buffer(on RAM)
                xQueueReset(s_uart_event_queue);    // Clear UART event queue
                break;

            case UART_BUFFER_FULL:
                // UART buffer full
                ESP_LOGW(TAG, "Ring buffer full, flushing RX");
                uart_flush_input(CONSOLE_UART_NUM);
                xQueueReset(s_uart_event_queue);
                break;
            default:
                ESP_LOGD(TAG, "Other UART event: %d", event.type);
                break;
            }
        }
    }
}



void app_main(void)
{
    ESP_LOGI(TAG, "Starting UART Command Console...");

    // Khởi tạo LED_strip
    led_strip_handle_t led_strip = led_strip_init_hardware();

    // Tắt LED ở trạng thái đầu
    led_strip_clear(led_strip);
    led_strip_refresh(led_strip);

    // Khởi tạo uart driver
    ESP_ERROR_CHECK(uart_init());
    // Khởi tạo task nhận dữ liệu từ UART (interrupt-drivent)
    xTaskCreate(uart_task, "uart_task", UART_TASK_STACK_SIZE, (void *)led_strip, UART_TASK_PRIORITY, NULL);
}
