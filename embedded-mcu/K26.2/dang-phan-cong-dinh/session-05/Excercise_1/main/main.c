/* =============================================================================
 * Interrupt-driven UART command console controlling the onboard RGB LED.
 *
 * BOARD: ESP32-S3-DevKitC-1 v1.1, RGB LED on GPIO38.
 *   >>> v1.0 boards have the LED on GPIO48. Check the silkscreen and change
 *   >>> RGB_LED_PIN below if yours is v1.0.
 *
 * Two physical channels are in play and they are NOT the same wire:
 *   - UART0 (GPIO43 TX / GPIO44 RX, through the CP2102N on the "UART" port)
 *     is the console. Characters typed there are echoed back there.
 *   - ESP_LOGx output goes to the USB Serial/JTAG peripheral on the "USB" port.
 *     This requires menuconfig -> Component config -> ESP System Settings
 *     -> Channel for console output -> USB Serial/JTAG Controller.
 *   Without that setting the framework's own logs are injected into UART0 and
 *   both streams turn to garbage.
 *
 * The RX path is interrupt-driven: uart_driver_install() is given an event
 * queue, and a dedicated task blocks on that queue. Nothing polls.
 * ========================================================================== */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "driver/uart.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "led_strip.h"

/* =============================================================================
 * 1. Named constants - UART
 * ========================================================================== */

#define UART_PORT_NUM       UART_NUM_0
#define UART_TX_PIN         GPIO_NUM_43
#define UART_RX_PIN         GPIO_NUM_44
#define UART_BAUD_RATE      (115200UL)
#define UART_BUF_SIZE       (1024U)   /* driver RX and TX ring buffers        */
#define UART_QUEUE_SIZE     (10U)     /* event queue depth                    */
#define UART_READ_CHUNK     (128U)    /* bytes pulled from the driver at once */

/* =============================================================================
 * 2. Named constants - command console
 * ========================================================================== */

#define CMD_BUF_SIZE        (64U)     /* including the terminating NUL        */
#define CMD_MAX_LEN         (CMD_BUF_SIZE - 1U)

#define CMD_LED_ON          "LED_ON"
#define CMD_LED_OFF         "LED_OFF"
#define CMD_RED             "RED"
#define CMD_GREEN           "GREEN"
#define CMD_BLUE            "BLUE"

/* ASCII control codes we care about */
#define ASCII_BACKSPACE     (0x08U)
#define ASCII_DEL           (0x7FU)
#define ASCII_CR            ('\r')
#define ASCII_LF            ('\n')

/* Sent back to erase one character on the operator's screen:
   move left, overwrite with a space, move left again. */
static const char ERASE_SEQUENCE[] = "\b \b";
static const char NEWLINE_SEQUENCE[] = "\r\n";

/* =============================================================================
 * 3. Named constants - RGB LED
 * ========================================================================== */

#define RGB_LED_PIN         GPIO_NUM_38  /* v1.0 boards: GPIO_NUM_48 */
#define RGB_LED_COUNT       (1U)         /* one WS2812 on the DevKitC-1 */
#define RMT_RESOLUTION_HZ   (10U * 1000U * 1000U)  /* 10 MHz, 0.1 us per tick */

/* WS2812 at full scale is uncomfortably bright on a desk. One named level,
   used for every colour, keeps the commands consistent. */
#define LED_LEVEL           (64U)
#define LED_OFF_LEVEL       (0U)

/* =============================================================================
 * 4. Named constants - task
 * ========================================================================== */

#define UART_TASK_STACK     (4096U)
#define UART_TASK_PRIO      (12U)
#define UART_TASK_CORE      (1)   /* APP CPU, keeps it off the protocol core */

static const char *TAG = "UART_CONSOLE";

/* =============================================================================
 * 5. RGB LED
 * ========================================================================== */

static led_strip_handle_t led_strip;

static void led_init(void)
{
    led_strip_config_t strip_config = {
        .strip_gpio_num   = RGB_LED_PIN,
        .max_leds         = RGB_LED_COUNT,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model        = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src       = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_RESOLUTION_HZ,
        .flags.with_dma = false,
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_ERROR_CHECK(led_strip_clear(led_strip));
}

static void led_set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    /* index 0 - there is only one pixel on this board */
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, r, g, b));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

/* =============================================================================
 * 6. Command handling
 * ========================================================================== */

static void handle_command(const char *cmd)
{
    /* An empty line (operator just pressed Enter) is not an error and not a
       command. Say nothing, do nothing. */
    if (cmd[0] == '\0') {
        return;
    }

    ESP_LOGI(TAG, "Received command: \"%s\"", cmd);

    if (strcmp(cmd, CMD_LED_ON) == 0) {
        led_set_rgb(LED_LEVEL, LED_LEVEL, LED_LEVEL);
        ESP_LOGI(TAG, "LED -> WHITE");
    } else if (strcmp(cmd, CMD_LED_OFF) == 0) {
        led_set_rgb(LED_OFF_LEVEL, LED_OFF_LEVEL, LED_OFF_LEVEL);
        ESP_LOGI(TAG, "LED -> OFF");
    } else if (strcmp(cmd, CMD_RED) == 0) {
        led_set_rgb(LED_LEVEL, LED_OFF_LEVEL, LED_OFF_LEVEL);
        ESP_LOGI(TAG, "LED -> RED");
    } else if (strcmp(cmd, CMD_GREEN) == 0) {
        led_set_rgb(LED_OFF_LEVEL, LED_LEVEL, LED_OFF_LEVEL);
        ESP_LOGI(TAG, "LED -> GREEN");
    } else if (strcmp(cmd, CMD_BLUE) == 0) {
        led_set_rgb(LED_OFF_LEVEL, LED_OFF_LEVEL, LED_LEVEL);
        ESP_LOGI(TAG, "LED -> BLUE");
    } else {
        /* Unknown command: warn, leave the LED exactly as it was, carry on.
           No reset, no hang - the console must survive operator typos. */
        ESP_LOGW(TAG, "Unknown command: \"%s\"", cmd);
    }
}

/* =============================================================================
 * 7. Line editor
 * ========================================================================== */

typedef struct {
    char     buf[CMD_BUF_SIZE];
    uint32_t len;       /* characters currently held, always <= CMD_MAX_LEN   */
    bool     overflow;  /* line grew past the buffer - discard it on Enter    */
} line_state_t;

static void line_reset(line_state_t *ls)
{
    ls->len      = 0U;
    ls->overflow = false;
    ls->buf[0]   = '\0';
}

static void uart_echo(const char *data, size_t len)
{
    uart_write_bytes(UART_PORT_NUM, data, len);
}

/* Feed one received byte into the line editor. */
static void line_feed_byte(line_state_t *ls, char c)
{
    if (c == ASCII_CR || c == ASCII_LF) {
        uart_echo(NEWLINE_SEQUENCE, strlen(NEWLINE_SEQUENCE));

        if (ls->overflow) {
            /* The line was longer than the buffer. Rejecting it is safer than
               acting on a truncated command that might match something. */
            ESP_LOGW(TAG, "Line too long (max %u), discarded", (unsigned)CMD_MAX_LEN);
        } else {
            ls->buf[ls->len] = '\0';   /* len <= CMD_MAX_LEN, so this is in range */
            handle_command(ls->buf);
        }
        line_reset(ls);
        return;
    }

    if ((uint8_t)c == ASCII_BACKSPACE || (uint8_t)c == ASCII_DEL) {
        if (ls->len > 0U) {
            ls->len--;
            uart_echo(ERASE_SEQUENCE, strlen(ERASE_SEQUENCE));
        }
        /* Nothing to erase: swallow the keystroke rather than echoing a
           backspace that would eat the prompt. */
        return;
    }

    if (isprint((unsigned char)c)) {
        if (ls->len < CMD_MAX_LEN) {
            ls->buf[ls->len] = c;
            ls->len++;
            uart_echo(&c, 1U);
        } else {
            /* Bounds check. The write above never happens past the end; from
               here on the line is doomed but the buffer stays intact. */
            ls->overflow = true;
        }
        return;
    }

    /* Any other control byte (arrow keys, tabs, stray binary) is ignored and
       deliberately not echoed. */
}

/* =============================================================================
 * 8. UART event task - the interrupt-driven RX path
 * ========================================================================== */

static QueueHandle_t uart_queue;

static void uart_event_task(void *arg)
{
    (void)arg;

    uart_event_t event;
    line_state_t line;
    uint8_t      chunk[UART_READ_CHUNK];

    line_reset(&line);

    ESP_LOGI(TAG, "Console ready on UART0, 115200-8-N-1");

    while (1) {
        /* Blocks forever. The UART ISR posts here when bytes arrive, so this
           task consumes no CPU while the operator is thinking. */
        if (xQueueReceive(uart_queue, &event, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        switch (event.type) {

        case UART_DATA: {
            size_t remaining = event.size;
            while (remaining > 0U) {
                size_t want = (remaining > sizeof(chunk)) ? sizeof(chunk) : remaining;
                int got = uart_read_bytes(UART_PORT_NUM, chunk, want, 0);
                if (got <= 0) {
                    break;   /* driver had less than advertised; stop cleanly */
                }
                for (int i = 0; i < got; i++) {
                    line_feed_byte(&line, (char)chunk[i]);
                }
                remaining -= (size_t)got;
            }
            break;
        }

        case UART_FIFO_OVF:
        case UART_BUFFER_FULL:
            /* Hardware FIFO or driver ring buffer overran - bytes were lost,
               so whatever is half-typed is no longer trustworthy. Throw away
               the input, drain the stale events, start the line from scratch. */
            uart_flush_input(UART_PORT_NUM);
            xQueueReset(uart_queue);
            line_reset(&line);
            ESP_LOGW(TAG, "UART overflow, input flushed");
            break;

        case UART_PARITY_ERR:
            ESP_LOGW(TAG, "UART parity error");
            break;

        case UART_FRAME_ERR:
            ESP_LOGW(TAG, "UART frame error");
            break;

        default:
            ESP_LOGI(TAG, "Unhandled UART event type: %d", (int)event.type);
            break;
        }
    }
}

/* =============================================================================
 * 9. app_main
 * ========================================================================== */

static void uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate  = UART_BAUD_RATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    /* The event queue is what makes this interrupt-driven: the driver's ISR
       posts an event here instead of us polling for bytes. */
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM,
                                        UART_BUF_SIZE,   /* RX ring buffer */
                                        UART_BUF_SIZE,   /* TX ring buffer */
                                        UART_QUEUE_SIZE,
                                        &uart_queue,
                                        0));
}

void app_main(void)
{
    led_init();
    uart_init();

    xTaskCreatePinnedToCore(uart_event_task, "uart_event", UART_TASK_STACK,
                            NULL, UART_TASK_PRIO, NULL, UART_TASK_CORE);
}
