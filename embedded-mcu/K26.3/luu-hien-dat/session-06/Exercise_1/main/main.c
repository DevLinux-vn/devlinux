#include <stddef.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LCD_SPI_HOST              SPI2_HOST
#define LCD_PIN_SCLK              GPIO_NUM_12
#define LCD_PIN_MOSI              GPIO_NUM_11
#define LCD_PIN_MISO              GPIO_NUM_13
#define LCD_PIN_CS                GPIO_NUM_10
#define LCD_PIN_DC                GPIO_NUM_9
#define LCD_PIN_RESET             GPIO_NUM_14
#define LCD_PIN_BACKLIGHT         GPIO_NUM_2

#define LCD_WIDTH                 480U
#define LCD_HEIGHT                320U
#define LCD_SPI_CLOCK_HZ          (20 * 1000 * 1000)
#define LCD_PIXEL_CHUNK_SIZE      320U
#define LCD_MAX_TRANSFER_SIZE     (LCD_PIXEL_CHUNK_SIZE * 2U)

#define LCD_CMD_SWRESET           0x01U
#define LCD_CMD_SLPOUT            0x11U
#define LCD_CMD_INVON             0x21U
#define LCD_CMD_DISPON             0x29U
#define LCD_CMD_CASET             0x2AU
#define LCD_CMD_RASET             0x2BU
#define LCD_CMD_RAMWR             0x2CU
#define LCD_CMD_MADCTL            0x36U
#define LCD_CMD_COLMOD            0x3AU

/* ST7796 MADCTL: landscape axis exchange and BGR color-order bit. */
#define LCD_MADCTL_LANDSCAPE_BGR  0xE8U
#define LCD_COLMOD_RGB565         0x55U

#define COLOR_BLACK               0x0000U
#define COLOR_WHITE               0xFFFFU
#define COLOR_RED                 0xF800U
#define COLOR_GREEN               0x07E0U
#define COLOR_BLUE                0x001FU

#define INITIAL_BARS_DURATION_MS  3000U
#define COLOR_HOLD_DURATION_MS    1000U

static const char *TAG = "st7796_spi";
static spi_device_handle_t lcd_spi;

static void lcd_write_command(uint8_t command)
{
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_DC, 0));

    spi_transaction_t transaction = {
        .length = 8,
        .tx_buffer = &command,
    };
    ESP_ERROR_CHECK(spi_device_polling_transmit(lcd_spi, &transaction));
}

static void lcd_write_data(const uint8_t *data, size_t length)
{
    if (length == 0U) {
        return;
    }

    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_DC, 1));

    spi_transaction_t transaction = {
        .length = length * 8U,
        .tx_buffer = data,
    };
    ESP_ERROR_CHECK(spi_device_polling_transmit(lcd_spi, &transaction));
}

static void lcd_set_address_window(uint16_t x_start, uint16_t y_start,
                                   uint16_t x_end, uint16_t y_end)
{
    const uint8_t column_data[] = {
        (uint8_t)(x_start >> 8), (uint8_t)x_start,
        (uint8_t)(x_end >> 8), (uint8_t)x_end,
    };
    const uint8_t row_data[] = {
        (uint8_t)(y_start >> 8), (uint8_t)y_start,
        (uint8_t)(y_end >> 8), (uint8_t)y_end,
    };

    lcd_write_command(LCD_CMD_CASET);
    lcd_write_data(column_data, sizeof(column_data));
    lcd_write_command(LCD_CMD_RASET);
    lcd_write_data(row_data, sizeof(row_data));
    lcd_write_command(LCD_CMD_RAMWR);
}

static void lcd_fill_rect(int x, int y, int width, int height, uint16_t colour)
{
    if (x < 0 || y < 0 || width <= 0 || height <= 0 ||
        x >= (int)LCD_WIDTH || y >= (int)LCD_HEIGHT) {
        return;
    }

    if (width > (int)LCD_WIDTH - x) {
        width = (int)LCD_WIDTH - x;
    }
    if (height > (int)LCD_HEIGHT - y) {
        height = (int)LCD_HEIGHT - y;
    }

    lcd_set_address_window((uint16_t)x, (uint16_t)y,
                           (uint16_t)(x + width - 1), (uint16_t)(y + height - 1));

    /* 320 RGB565 pixels = 640 bytes per SPI transfer; bounds DMA and stack use. */
    uint8_t pixel_chunk[LCD_MAX_TRANSFER_SIZE];
    for (size_t pixel = 0; pixel < LCD_PIXEL_CHUNK_SIZE; ++pixel) {
        pixel_chunk[pixel * 2U] = (uint8_t)(colour >> 8);
        pixel_chunk[pixel * 2U + 1U] = (uint8_t)colour;
    }

    size_t pixels_remaining = (size_t)width * (size_t)height;
    while (pixels_remaining > 0U) {
        const size_t pixels_this_transfer =
            (pixels_remaining < LCD_PIXEL_CHUNK_SIZE) ? pixels_remaining : LCD_PIXEL_CHUNK_SIZE;
        lcd_write_data(pixel_chunk, pixels_this_transfer * 2U);
        pixels_remaining -= pixels_this_transfer;
    }
}

static void lcd_fill_screen(uint16_t colour)
{
    lcd_fill_rect(0, 0, (int)LCD_WIDTH, (int)LCD_HEIGHT, colour);
}

static void lcd_initialize_gpio(void)
{
    const gpio_config_t output_config = {
        .pin_bit_mask = (1ULL << LCD_PIN_DC) |
                        (1ULL << LCD_PIN_RESET) |
                        (1ULL << LCD_PIN_BACKLIGHT),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&output_config));
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_BACKLIGHT, 1));
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_DC, 1));
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_RESET, 1));
}

static void lcd_initialize_spi(void)
{
    const spi_bus_config_t bus_config = {
        .mosi_io_num = LCD_PIN_MOSI,
        .miso_io_num = LCD_PIN_MISO,
        .sclk_io_num = LCD_PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_MAX_TRANSFER_SIZE,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO));

    const spi_device_interface_config_t device_config = {
        .clock_speed_hz = LCD_SPI_CLOCK_HZ,
        .mode = 0,
        .spics_io_num = LCD_PIN_CS,
        .queue_size = 1,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(LCD_SPI_HOST, &device_config, &lcd_spi));
}

static void lcd_hardware_reset(void)
{
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_RESET, 0));
    vTaskDelay(pdMS_TO_TICKS(20));
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_RESET, 1));
    vTaskDelay(pdMS_TO_TICKS(120));
}

static void lcd_initialize_controller(void)
{
    lcd_write_command(LCD_CMD_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));

    lcd_write_command(LCD_CMD_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(120));

    const uint8_t madctl = LCD_MADCTL_LANDSCAPE_BGR;
    lcd_write_command(LCD_CMD_MADCTL);
    lcd_write_data(&madctl, sizeof(madctl));

    const uint8_t colmod = LCD_COLMOD_RGB565;
    lcd_write_command(LCD_CMD_COLMOD);
    lcd_write_data(&colmod, sizeof(colmod));

    lcd_write_command(LCD_CMD_INVON);
    lcd_write_command(LCD_CMD_DISPON);
    vTaskDelay(pdMS_TO_TICKS(50));
}

static void show_three_colour_bars(void)
{
    const int first_bar_height = (int)(LCD_HEIGHT / 3U);
    const int second_bar_height = (int)(LCD_HEIGHT / 3U);
    const int third_bar_height = (int)LCD_HEIGHT - first_bar_height - second_bar_height;

    lcd_fill_rect(0, 0, (int)LCD_WIDTH, first_bar_height, COLOR_RED);
    lcd_fill_rect(0, first_bar_height, (int)LCD_WIDTH, second_bar_height, COLOR_GREEN);
    lcd_fill_rect(0, first_bar_height + second_bar_height,
                  (int)LCD_WIDTH, third_bar_height, COLOR_BLUE);
    vTaskDelay(pdMS_TO_TICKS(INITIAL_BARS_DURATION_MS));
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initialising ST7796U for %ux%u landscape RGB565",
             (unsigned int)LCD_WIDTH, (unsigned int)LCD_HEIGHT);

    lcd_initialize_gpio();
    lcd_initialize_spi();
    lcd_hardware_reset();
    lcd_initialize_controller();

    show_three_colour_bars();

    while (true) {
        lcd_fill_screen(COLOR_RED);
        vTaskDelay(pdMS_TO_TICKS(COLOR_HOLD_DURATION_MS));
        lcd_fill_screen(COLOR_GREEN);
        vTaskDelay(pdMS_TO_TICKS(COLOR_HOLD_DURATION_MS));
        lcd_fill_screen(COLOR_BLUE);
        vTaskDelay(pdMS_TO_TICKS(COLOR_HOLD_DURATION_MS));
        lcd_fill_screen(COLOR_WHITE);
        vTaskDelay(pdMS_TO_TICKS(COLOR_HOLD_DURATION_MS));
        lcd_fill_screen(COLOR_BLACK);
        vTaskDelay(pdMS_TO_TICKS(COLOR_HOLD_DURATION_MS));
    }
}
