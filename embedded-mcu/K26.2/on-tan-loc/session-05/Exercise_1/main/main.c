#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "driver/spi_common.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "hal/spi_types.h"
#include "sdkconfig.h"

#ifndef CONFIG_LCD_MISO_GPIO
#define CONFIG_LCD_MISO_GPIO 13
#endif

#ifndef CONFIG_LCD_SCLK_GPIO
#define CONFIG_LCD_SCLK_GPIO 12
#endif

#ifndef CONFIG_LCD_MOSI_GPIO
#define CONFIG_LCD_MOSI_GPIO 11
#endif

#ifndef CONFIG_LCD_CS_GPIO
#define CONFIG_LCD_CS_GPIO 10
#endif

#ifndef CONFIG_LCD_DC_GPIO
#define CONFIG_LCD_DC_GPIO 9
#endif

#ifndef CONFIG_LCD_RST_GPIO
#define CONFIG_LCD_RST_GPIO 14
#endif

#ifndef CONFIG_LCD_BLK_GPIO
#define CONFIG_LCD_BLK_GPIO 2
#endif

#ifndef CONFIG_LCD_WIDTH
#define CONFIG_LCD_WIDTH 480
#endif

#ifndef CONFIG_LCD_HEIGHT
#define CONFIG_LCD_HEIGHT 320
#endif

#ifndef CONFIG_LCD_SPI_CLOCK_HZ
#define CONFIG_LCD_SPI_CLOCK_HZ (20 * 1000 * 1000)
#endif

/* RGB565 colours display in pixel */
#define COLOUR_RED    (0xF800U)
#define COLOUR_GREEN  (0x07E0U)
#define COLOUR_BLUE   (0x001FU)
#define COLOUR_WHITE  (0xFFFFU)
#define COLOUR_BLACK  (0x0000U)

/* cmd */
#define CMD_SWRESET   (0x01U)
#define CMD_SLPOUT    (0x11U) /* quit sleep mode wait 120ms */
#define CMD_INVON     (0x21U) /* invert display */
#define CMD_DISPON    (0x29U) /* allow to display */
#define CMD_CASET     (0x2AU) /* config width */
#define CMD_RASET     (0x2BU) /* config height */
#define CMD_RAMWR     (0x2CU) /* write into RAM, distribute color to pixel */
#define CMD_MADCTL    (0x36U) /* config direction display */
#define CMD_COLMOD    (0x3AU) /* coulor standard */

/* MADCTL bits */
#define MADCTL_MY     (0x80U) /* row address order */
#define MADCTL_MX     (0x40U) /* column address order */
#define MADCTL_MV     (0x20U) /* row/column exchange -> landscape */
#define MADCTL_BGR    (0x08U) /* colour order: set = BGR, clear = RGB */

/* change little end to MSB first order */
typedef struct {
    uint8_t msb;
    uint8_t lsb;
} msb_first_order;

static bool BLK_ON = true;
static const char *TAG = "ST7796_DRIVER";
static spi_device_handle_t spi_handle = NULL;

msb_first_order change_order(uint16_t value_little_end) {
    msb_first_order result;
    result.msb = (uint8_t)((value_little_end >> 8) & 0xFF);
    result.lsb = (uint8_t)(value_little_end & 0xFF);
    return result;
}

esp_err_t lcd_write_data(const uint8_t *data, size_t len) {
    if (data == NULL || len == 0) {
        return ESP_OK;
    }

    if (CONFIG_LCD_DC_GPIO >= 0) {
        ESP_ERROR_CHECK(gpio_set_level(CONFIG_LCD_DC_GPIO, 1));
    }

    spi_transaction_t transaction_data;
    memset(&transaction_data, 0, sizeof(transaction_data));

    transaction_data.length = len * 8;
    transaction_data.tx_buffer = data;

    return spi_device_polling_transmit(spi_handle, &transaction_data);
}

esp_err_t lcd_write_cmd(uint8_t cmd) {
    if (CONFIG_LCD_DC_GPIO >= 0) {
        ESP_ERROR_CHECK(gpio_set_level(CONFIG_LCD_DC_GPIO, 0)); // DC = 0 (Command)
    }

    spi_transaction_t transaction_cmd;
    memset(&transaction_cmd, 0, sizeof(transaction_cmd));

    transaction_cmd.length = 8;
    transaction_cmd.tx_buffer = &cmd;

    return spi_device_polling_transmit(spi_handle, &transaction_cmd);
}

esp_err_t lcd_set_window(uint16_t x_start, uint16_t y_start,
                         uint16_t x_end, uint16_t y_end) {

    msb_first_order x_start_softed = change_order(x_start);
    msb_first_order x_end_softed   = change_order(x_end);

    /* CASET - Set Column Address */
    ESP_ERROR_CHECK(lcd_write_cmd((uint8_t) CMD_CASET));
    ESP_ERROR_CHECK(lcd_write_data(&x_start_softed.msb, sizeof(uint8_t)));
    ESP_ERROR_CHECK(lcd_write_data(&x_start_softed.lsb, sizeof(uint8_t)));
    ESP_ERROR_CHECK(lcd_write_data(&x_end_softed.msb,   sizeof(uint8_t)));
    ESP_ERROR_CHECK(lcd_write_data(&x_end_softed.lsb,   sizeof(uint8_t)));

    msb_first_order y_start_softed = change_order(y_start);
    msb_first_order y_end_softed   = change_order(y_end);
    
    /* RASET - Set Row Address */
    ESP_ERROR_CHECK(lcd_write_cmd((uint8_t) CMD_RASET));
    ESP_ERROR_CHECK(lcd_write_data(&y_start_softed.msb, sizeof(uint8_t)));
    ESP_ERROR_CHECK(lcd_write_data(&y_start_softed.lsb, sizeof(uint8_t)));
    ESP_ERROR_CHECK(lcd_write_data(&y_end_softed.msb,   sizeof(uint8_t)));
    ESP_ERROR_CHECK(lcd_write_data(&y_end_softed.lsb,   sizeof(uint8_t)));

    return lcd_write_cmd((uint8_t) CMD_RAMWR);
}

esp_err_t lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if ((x >= CONFIG_LCD_WIDTH) || (y >= CONFIG_LCD_HEIGHT) || (w == 0) || (h == 0)) {
        return ESP_FAIL;
    }

    if (x + w > CONFIG_LCD_WIDTH) {
        w = CONFIG_LCD_WIDTH - x;
    }
    if (y + h > CONFIG_LCD_HEIGHT) {
        h = CONFIG_LCD_HEIGHT - y;
    }

    ESP_ERROR_CHECK(lcd_set_window(x, y, x + w - 1, y + h - 1));

    uint16_t color_be = (color >> 8) | (color << 8);
    static uint16_t line_buffer[480];
    uint16_t line_len = (w > 480) ? 480 : w;

    for (int i = 0; i < line_len; i++) {
        line_buffer[i] = color_be;
    }

    for (int row = 0; row < h; row++) {
        ESP_ERROR_CHECK(lcd_write_data((uint8_t*) line_buffer, line_len * sizeof(uint16_t)));
    }

    return ESP_OK;
}

esp_err_t lcd_fill_screen(uint16_t colour) {
    return lcd_fill_rect(0, 0, CONFIG_LCD_WIDTH, CONFIG_LCD_HEIGHT, colour);
}

esp_err_t lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    if ((x >= CONFIG_LCD_WIDTH) || (y >= CONFIG_LCD_HEIGHT)) {
        return ESP_FAIL;
    }
    ESP_ERROR_CHECK(lcd_set_window(x, y, x, y));
    
    uint8_t data[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    return lcd_write_data(data, sizeof(data));
}

esp_err_t lcd_init(void) { 
    ESP_LOGI(TAG, "MISO: %d | MOSI: %d | SCLK: %d | CS: %d", 
             CONFIG_LCD_MISO_GPIO, CONFIG_LCD_MOSI_GPIO, 
             CONFIG_LCD_SCLK_GPIO, CONFIG_LCD_CS_GPIO);

    spi_bus_config_t bus_config = {
        .miso_io_num = CONFIG_LCD_MISO_GPIO,
        .mosi_io_num = CONFIG_LCD_MOSI_GPIO,
        .sclk_io_num = CONFIG_LCD_SCLK_GPIO,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = CONFIG_LCD_WIDTH * sizeof(uint16_t) * 2,
    };

    spi_device_interface_config_t dev_config = {
        .mode = 0,
        .clock_speed_hz = CONFIG_LCD_SPI_CLOCK_HZ,
        .spics_io_num = CONFIG_LCD_CS_GPIO,
        .queue_size = 7,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_config, &spi_handle));

    uint64_t pin_mask = 0;
    if (CONFIG_LCD_DC_GPIO >= 0)  pin_mask |= (1ULL << CONFIG_LCD_DC_GPIO);
    if (CONFIG_LCD_BLK_GPIO >= 0) pin_mask |= (1ULL << CONFIG_LCD_BLK_GPIO);
    if (CONFIG_LCD_RST_GPIO >= 0) pin_mask |= (1ULL << CONFIG_LCD_RST_GPIO);

    if (pin_mask > 0) {
        gpio_config_t pGPIOConfig = {
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE, 
            .intr_type = GPIO_INTR_DISABLE,
            .pin_bit_mask = pin_mask,
        };
        ESP_ERROR_CHECK(gpio_config(&pGPIOConfig));
    }

    if (CONFIG_LCD_RST_GPIO >= 0) {
        ESP_ERROR_CHECK(gpio_set_level(CONFIG_LCD_RST_GPIO, 0));
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_ERROR_CHECK(gpio_set_level(CONFIG_LCD_RST_GPIO, 1));
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    if (CONFIG_LCD_BLK_GPIO >= 0) {
        ESP_ERROR_CHECK(gpio_set_level(CONFIG_LCD_BLK_GPIO, BLK_ON));
    }

    ESP_ERROR_CHECK(lcd_write_cmd(CMD_SWRESET)); 
    vTaskDelay(pdMS_TO_TICKS(120));

    ESP_ERROR_CHECK(lcd_write_cmd(CMD_SLPOUT)); 
    vTaskDelay(pdMS_TO_TICKS(120));

    ESP_ERROR_CHECK(lcd_write_cmd(CMD_COLMOD));
    uint8_t colmod = 0x55;
    ESP_ERROR_CHECK(lcd_write_data(&colmod, 1));

    ESP_ERROR_CHECK(lcd_write_cmd(CMD_MADCTL)); 
    uint8_t madctl = MADCTL_MV | MADCTL_MY | MADCTL_BGR;
    ESP_ERROR_CHECK(lcd_write_data(&madctl, 1));

    ESP_ERROR_CHECK(lcd_write_cmd(CMD_INVON));

    ESP_ERROR_CHECK(lcd_write_cmd(CMD_DISPON));
    vTaskDelay(pdMS_TO_TICKS(20));

    return ESP_OK;
}

void app_main(void) {
    ESP_ERROR_CHECK(lcd_init());

    uint16_t bar_height = CONFIG_LCD_HEIGHT / 3;

    ESP_ERROR_CHECK(lcd_fill_rect(0, 0, CONFIG_LCD_WIDTH, bar_height, COLOUR_RED));
    ESP_ERROR_CHECK(lcd_fill_rect(0, bar_height, CONFIG_LCD_WIDTH, bar_height, COLOUR_GREEN));
    ESP_ERROR_CHECK(lcd_fill_rect(0, bar_height * 2, CONFIG_LCD_WIDTH, CONFIG_LCD_HEIGHT - (bar_height * 2), COLOUR_BLUE));

    vTaskDelay(pdMS_TO_TICKS(3000));

    uint16_t display_colors[5] = {COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE, COLOUR_WHITE, COLOUR_BLACK};

    int color_index = 0;
    while (1) {
        ESP_ERROR_CHECK(lcd_fill_screen(display_colors[color_index]));
        color_index = (color_index + 1) % 5;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}