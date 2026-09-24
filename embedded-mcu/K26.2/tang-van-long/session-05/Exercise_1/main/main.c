#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_err.h"

#include "led_strip.h"


/* ============================================================
 * UART configuration
 * ============================================================ */

#define UART_PORT_NUM       UART_NUM_0
#define UART_TX_PIN         GPIO_NUM_43
#define UART_RX_PIN         GPIO_NUM_44
#define UART_BAUD_RATE      115200UL

#define UART_BUF_SIZE       1024U
#define UART_QUEUE_SIZE     10U
#define UART_READ_SIZE      128U


/* ============================================================
 * Command buffer
 * ============================================================ */

#define CMD_BUF_SIZE        64U


/* ============================================================
 * RGB LED
 * ============================================================ */

#define RGB_LED_PIN         GPIO_NUM_48
#define RGB_LED_COUNT       1U
#define RGB_LED_INDEX       0U


/* ============================================================
 * Command strings
 * ============================================================ */

#define CMD_LED_ON          "LED_ON"
#define CMD_LED_OFF         "LED_OFF"
#define CMD_RED             "RED"
#define CMD_GREEN           "GREEN"
#define CMD_BLUE            "BLUE"


/* ============================================================
 * Task configuration
 * ============================================================ */

#define UART_TASK_STACK_SIZE    4096U
#define UART_TASK_PRIORITY      5U
#define UART_TASK_CORE          1


/* ============================================================
 * Log tag
 * ============================================================ */

static const char *TAG = "UART_CONSOLE";


/* ============================================================
 * Global objects
 * ============================================================ */

static QueueHandle_t uart_queue = NULL;

static led_strip_handle_t led_strip = NULL;


/* ============================================================
 * UART console state
 * ============================================================ */

static char cmd_buf[CMD_BUF_SIZE];

static size_t cmd_len = 0;

static bool cmd_overflow = false;

static bool ignore_next_lf = false;


/* ============================================================
 * RGB LED functions
 * ============================================================ */

static void led_set_color(uint8_t red, uint8_t green, uint8_t blue)
{
    ESP_ERROR_CHECK( led_strip_set_pixel(led_strip, RGB_LED_INDEX, red, green, blue));
    ESP_ERROR_CHECK( led_strip_refresh(led_strip));
}


static void led_init(void)
{
    led_strip_config_t strip_config =
    {
        .strip_gpio_num = RGB_LED_PIN,
        .max_leds = RGB_LED_COUNT,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags =
        {
            .invert_out = false
        }
    };

    led_strip_rmt_config_t rmt_config =
    {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .flags =
        {
            .with_dma = false
        }
    };

    ESP_ERROR_CHECK( led_strip_new_rmt_device( &strip_config, &rmt_config, &led_strip));
    ESP_ERROR_CHECK( led_strip_clear(led_strip));
}


/* ============================================================
 * UART functions
 * ============================================================ */

static void uart_send_string(const char *str)
{
    uart_write_bytes( UART_PORT_NUM, str, strlen(str));
}


/* ============================================================
 * Command buffer
 * ============================================================ */

static void command_reset(void)
{
    cmd_len = 0;
    cmd_overflow = false;
    memset(cmd_buf,0,sizeof(cmd_buf));
}


/* ============================================================
 * Command handler
 * ============================================================ */

static void handle_command(const char *cmd)
{
    ESP_LOGI(TAG,"Received command: \"%s\"",cmd);

    if (strcmp(cmd, CMD_LED_ON) == 0)
    {
        led_set_color(255, 255, 255);
        ESP_LOGI(TAG,"LED -> WHITE");
        return;
    }

    if (strcmp(cmd, CMD_LED_OFF) == 0)
    {
        led_set_color(0, 0, 0);
        ESP_LOGI(TAG,"LED -> OFF");
        return;
    }

    if (strcmp(cmd, CMD_RED) == 0)
    {
        led_set_color(255, 0, 0);
        ESP_LOGI(TAG,"LED -> RED");
        return;
    }

    if (strcmp(cmd, CMD_GREEN) == 0)
    {
        led_set_color(0, 255, 0);
        ESP_LOGI(TAG,"LED -> GREEN");
        return;
    }

    if (strcmp(cmd, CMD_BLUE) == 0)
    {
        led_set_color(0, 0, 255);
        ESP_LOGI(TAG,"LED -> BLUE");
        return;
    }

    ESP_LOGW(TAG,"Unknown command: \"%s\"",cmd);
}

static void process_character(uint8_t ch)
{

    if (ch == '\r')
    {
        uart_send_string("\r\n");

        if (cmd_overflow)
        {
            ESP_LOGW(TAG,"Command too long, input rejected");
        }
        else
        {
            cmd_buf[cmd_len] = '\0';
            if (cmd_len > 0)
            {
                handle_command(cmd_buf);
            }
        }
        command_reset();
        ignore_next_lf = true;

        return;
    }

    if (ch == '\n')
    {
        if (ignore_next_lf)
        {
            ignore_next_lf = false;
            return;
        }

        uart_send_string("\r\n");

        if (cmd_overflow)
        {
            ESP_LOGW(TAG,"Command too long, input rejected");
        }
        else
        {
            cmd_buf[cmd_len] = '\0';
            if (cmd_len > 0)
            {
                handle_command(cmd_buf);
            }
        }
        command_reset();

        return;
    }

    ignore_next_lf = false;
    if ((ch == '\b') || (ch == 127))
    {
        if (cmd_len > 0)
        {
            cmd_len--;
            cmd_buf[cmd_len] = '\0';
            uart_send_string("\b \b");
        }

        return;
    }

    if (isprint(ch))
    {
        if (cmd_len < (CMD_BUF_SIZE - 1))
        {
            cmd_buf[cmd_len] = (char)ch;
            cmd_len++;
            cmd_buf[cmd_len] = '\0';
            uart_write_bytes(UART_PORT_NUM,&ch,1);
        }
        else
        {
            if (!cmd_overflow)
            {
                ESP_LOGW(TAG,"Command buffer full");
                uint8_t bell = '\a';
                uart_write_bytes(UART_PORT_NUM,&bell,1);
            }
            cmd_overflow = true;
        }

        return;
    }
}

static void uart_event_task(void *arg)
{
    uart_event_t event;
    uint8_t data[UART_READ_SIZE];
    while (1)
    {
        if (xQueueReceive(uart_queue,&event,portMAX_DELAY) != pdTRUE)
        {
            continue;
        }

        switch (event.type)
        {
            case UART_DATA:
            {
                size_t remaining = event.size;
                while (remaining > 0)
                {
                    size_t read_size = remaining;

                    if (read_size > UART_READ_SIZE)
                    {
                        read_size = UART_READ_SIZE;
                    }

                    int len = uart_read_bytes(UART_PORT_NUM,data,read_size,portMAX_DELAY);
                    if (len <= 0)
                    {
                        break;
                    }

                    for (int i = 0; i < len; i++)
                    {
                        process_character(data[i]);
                    }

                    remaining -= len;
                }

                break;
            }
            case UART_FIFO_OVF:
            {
                ESP_LOGW(TAG,"UART FIFO overflow, input flushed");
                uart_flush_input(UART_PORT_NUM);
                xQueueReset(uart_queue);
                command_reset();
                ignore_next_lf = false;
                break;
            }
            case UART_BUFFER_FULL:
            {
                ESP_LOGW(TAG,"UART buffer full, input flushed");
                uart_flush_input(UART_PORT_NUM);
                xQueueReset(uart_queue);
                command_reset();
                ignore_next_lf = false;
                break;
            }
            default:
            {
                ESP_LOGW(TAG,"Unhandled UART event: %d",event.type);
                break;
            }
        }
    }
}

static void uart_console_init(void)
{
    uart_config_t uart_config =
    {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM,&uart_config));
    ESP_ERROR_CHECK( uart_set_pin( UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK( uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE, UART_BUF_SIZE, UART_QUEUE_SIZE, &uart_queue,0));
}

void app_main(void)
{
    led_init();
    uart_console_init();
    BaseType_t result = xTaskCreatePinnedToCore( uart_event_task, "uart_event_task", UART_TASK_STACK_SIZE, NULL, UART_TASK_PRIORITY, NULL, UART_TASK_CORE);
    if (result != pdPASS)
    {
        ESP_LOGE(TAG,"Failed to create UART event task");
        abort();
    }
    ESP_LOGI(TAG,"Console ready on UART0, 115200-8-N-1");
}