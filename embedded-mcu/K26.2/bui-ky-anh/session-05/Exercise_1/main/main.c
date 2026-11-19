/* ESP32-S3 DevKitC-1 v1.0, onboard WS2812 RGB LED on GPIO48.
 * UART0 console: CP2102N USB/UART connector, TX GPIO43 / RX GPIO44.
 * ESP_LOG output: native USB Serial/JTAG connector (sdkconfig.defaults).
 */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "led_strip.h"

#if !CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
#error "Select USB Serial/JTAG for the ESP-IDF console; UART0 belongs to this application."
#endif

#define UART_PORT_NUM        UART_NUM_0
#define UART_TX_PIN          GPIO_NUM_43
#define UART_RX_PIN          GPIO_NUM_44
#define UART_BAUD_RATE       115200
#define UART_BUF_SIZE        1024U
#define UART_QUEUE_SIZE      10U
#define RX_CHUNK_SIZE        128U
#define CMD_BUF_SIZE         64U
#define RGB_LED_PIN          GPIO_NUM_48
#define RGB_LED_COUNT        1U
#define RGB_BRIGHTNESS       16U
#define RMT_RESOLUTION_HZ    10000000U
#define UART_TASK_STACK      4096U
#define UART_TASK_PRIORITY   12U
#define UART_TASK_CORE       0
#define RX_READ_TIMEOUT_MS   20U
#define PRINTABLE_FIRST      0x20U
#define PRINTABLE_LAST       0x7EU
#define CHAR_CR              '\r'
#define CHAR_LF              '\n'
#define CHAR_BACKSPACE       '\b'
#define CHAR_DEL             0x7FU

static const char CMD_LED_ON[]  = "LED_ON";
static const char CMD_LED_OFF[] = "LED_OFF";
static const char CMD_RED[]     = "RED";
static const char CMD_GREEN[]   = "GREEN";
static const char CMD_BLUE[]    = "BLUE";
static const char ECHO_NEWLINE[] = "\r\n";
static const char ECHO_ERASE[]   = "\b \b";
static const char *TAG = "UART_CONSOLE";
static QueueHandle_t uart_queue;
static led_strip_handle_t led_strip;

static void rgb_set(uint8_t red, uint8_t green, uint8_t blue)
{
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0U, red, green, blue));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

static void rgb_init(void)
{
    const led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_LED_PIN,
        .max_leds = RGB_LED_COUNT,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };
    const led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_RESOLUTION_HZ,
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_ERROR_CHECK(led_strip_clear(led_strip));
}

static void handle_command(const char *command)
{
    ESP_LOGI(TAG, "Received command: \"%s\"", command);
    if (strcmp(command, CMD_LED_ON) == 0) {
        rgb_set(RGB_BRIGHTNESS, RGB_BRIGHTNESS, RGB_BRIGHTNESS);
        ESP_LOGI(TAG, "LED -> WHITE");
    } else if (strcmp(command, CMD_LED_OFF) == 0) {
        ESP_ERROR_CHECK(led_strip_clear(led_strip));
        ESP_LOGI(TAG, "LED -> OFF");
    } else if (strcmp(command, CMD_RED) == 0) {
        rgb_set(RGB_BRIGHTNESS, 0U, 0U);
        ESP_LOGI(TAG, "LED -> RED");
    } else if (strcmp(command, CMD_GREEN) == 0) {
        rgb_set(0U, RGB_BRIGHTNESS, 0U);
        ESP_LOGI(TAG, "LED -> GREEN");
    } else if (strcmp(command, CMD_BLUE) == 0) {
        rgb_set(0U, 0U, RGB_BRIGHTNESS);
        ESP_LOGI(TAG, "LED -> BLUE");
    } else {
        ESP_LOGW(TAG, "Unknown command: \"%s\"", command);
    }
}

typedef struct {
    char command[CMD_BUF_SIZE];
    size_t length;
    bool discard_line;
    bool previous_cr;
} line_state_t;

static void echo_bytes(const char *bytes, size_t length)
{
    (void)uart_write_bytes(UART_PORT_NUM, bytes, length);
}

static void reset_line(line_state_t *line)
{
    *line = (line_state_t){0};
}

static void receive_byte(line_state_t *line, uint8_t byte)
{
    if (byte == CHAR_CR || byte == CHAR_LF) {
        /* Treat CRLF as one Enter, including when split across UART events. */
        if (byte == CHAR_LF && line->previous_cr) {
            line->previous_cr = false;
            return;
        }
        echo_bytes(ECHO_NEWLINE, sizeof(ECHO_NEWLINE) - 1U);
        if (!line->discard_line && line->length != 0U) {
            line->command[line->length] = '\0';
            handle_command(line->command);
        }
        const bool was_cr = byte == CHAR_CR;
        reset_line(line);
        line->previous_cr = was_cr;
        return;
    }
    line->previous_cr = false;
    if (byte == CHAR_BACKSPACE || byte == CHAR_DEL) {
        if (!line->discard_line && line->length != 0U) {
            --line->length;
            line->command[line->length] = '\0';
            echo_bytes(ECHO_ERASE, sizeof(ECHO_ERASE) - 1U);
        }
        return;
    }
    const char character = (char)byte;
    echo_bytes(&character, sizeof(character));
    if (line->discard_line) {
        return;
    }
    if (byte < PRINTABLE_FIRST || byte > PRINTABLE_LAST) {
        line->discard_line = true;
        ESP_LOGW(TAG, "Non-printable input: line rejected; press Enter");
    } else if (line->length < CMD_BUF_SIZE - 1U) {
        line->command[line->length++] = character;
        line->command[line->length] = '\0';
    } else {
        /* Reject the WHOLE line until Enter; never execute its truncated prefix. */
        line->discard_line = true;
        ESP_LOGW(TAG, "Command exceeds %u characters: line rejected; press Enter",
                 (unsigned)(CMD_BUF_SIZE - 1U));
    }
}

static void recover_uart(line_state_t *line, const char *reason)
{
    ESP_ERROR_CHECK(uart_flush_input(UART_PORT_NUM));
    (void)xQueueReset(uart_queue);
    reset_line(line);
    echo_bytes(ECHO_NEWLINE, sizeof(ECHO_NEWLINE) - 1U);
    ESP_LOGW(TAG, "%s: input flushed, line reset", reason);
}

static void uart_event_task(void *arg)
{
    (void)arg;
    uart_event_t event;
    uint8_t data[RX_CHUNK_SIZE];
    line_state_t line = {0};
    for (;;) {
        if (xQueueReceive(uart_queue, &event, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        switch (event.type) {
        case UART_DATA: {
            size_t remaining = event.size;
            while (remaining != 0U) {
                const size_t requested = remaining < sizeof(data) ? remaining : sizeof(data);
                const int received = uart_read_bytes(UART_PORT_NUM, data, requested,
                                                     pdMS_TO_TICKS(RX_READ_TIMEOUT_MS));
                if (received <= 0) {
                    recover_uart(&line, "UART read incomplete");
                    break;
                }
                for (int i = 0; i < received; ++i) {
                    receive_byte(&line, data[i]);
                }
                remaining -= (size_t)received;
            }
            break;
        }
        case UART_FIFO_OVF:
            recover_uart(&line, "UART_FIFO_OVF");
            break;
        case UART_BUFFER_FULL:
            recover_uart(&line, "UART_BUFFER_FULL");
            break;
        case UART_PARITY_ERR:
        case UART_FRAME_ERR:
            recover_uart(&line, "UART parity/frame error");
            break;
        default:
            ESP_LOGD(TAG, "Ignored UART event: %d", (int)event.type);
            break;
        }
    }
}

void app_main(void)
{
    rgb_init();
    const uart_config_t config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE, UART_BUF_SIZE,
                                        UART_QUEUE_SIZE, &uart_queue, 0));
    ESP_ERROR_CHECK(xTaskCreatePinnedToCore(uart_event_task, "uart_event_task",
                                           UART_TASK_STACK, NULL, UART_TASK_PRIORITY,
                                           NULL, UART_TASK_CORE) == pdPASS
                    ? ESP_OK : ESP_ERR_NO_MEM);
    ESP_LOGI(TAG, "Console ready on UART0, %d-8-N-1; RGB GPIO%d",
             UART_BAUD_RATE, (int)RGB_LED_PIN);
}
