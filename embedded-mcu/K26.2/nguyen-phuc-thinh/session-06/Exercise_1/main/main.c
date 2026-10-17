/*
 * ST7796U SPI Display Driver (No esp_lcd)
 * Orientation: Landscape (480x320)
 * Colour Order: BGR (Fixed via MADCTL)
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"

/* --- HARDWARE & PINS --- */
#define LCD_HOST       SPI2_HOST
#define PIN_SCK        GPIO_NUM_12
#define PIN_MOSI       GPIO_NUM_11
#define PIN_MISO       GPIO_NUM_13
#define PIN_CS         GPIO_NUM_10
#define PIN_RS         GPIO_NUM_9
#define PIN_RST        GPIO_NUM_14
#define PIN_BK_LIGHT   GPIO_NUM_2

/* --- DISPLAY CONSTANTS --- */
#define LCD_H_RES      (480U)
#define LCD_V_RES      (320U)
#define LCD_CLK_HZ     (20 * 1000 * 1000)

/* Chunk size: 10 lines of pixels. 
 * Justification: 480 * 10 * 2 bytes = 9600 bytes. 
 * Small enough to fit in internal SRAM, large enough to keep SPI DMA busy and avoid transaction overhead. */
#define CHUNK_PIXELS   (LCD_H_RES * 10)
#define CHUNK_BYTES    (CHUNK_PIXELS * 2)

/* --- COMMANDS & REGISTERS --- */
#define CMD_SWRESET    (0x01U)
#define CMD_SLPOUT     (0x11U)
#define CMD_INVON      (0x21U)
#define CMD_DISPON     (0x29U)
#define CMD_CASET      (0x2AU)
#define CMD_RASET      (0x2BU)
#define CMD_RAMWR      (0x2CU)
#define CMD_MADCTL     (0x36U)
#define CMD_COLMOD     (0x3AU)

#define MADCTL_MV      (0x20U) /* Row/column exchange (Landscape) */
#define MADCTL_BGR     (0x08U) /* BGR color order - fixes red/blue swap */
#define LCD_MADCTL_VAL (MADCTL_MV | MADCTL_BGR)

/* --- COLOURS (RGB565) --- */
#define COLOUR_RED     (0xF800U)
#define COLOUR_GREEN   (0x07E0U)
#define COLOUR_BLUE    (0x001FU)
#define COLOUR_WHITE   (0xFFFFU)
#define COLOUR_BLACK   (0x0000U)

/* --- GLOBALS --- */
static spi_device_handle_t spi;
static uint8_t *tx_buffer;

/* --- LOW-LEVEL SPI HELPERS --- */
static void lcd_write_cmd(uint8_t cmd) {
    ESP_ERROR_CHECK(gpio_set_level(PIN_RS, 0)); /* Command mode */
    spi_transaction_t t = { .length = 8, .tx_buffer = &cmd };
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));
}

static void lcd_write_data(const uint8_t *data, size_t len) {
    if (len == 0) return;
    ESP_ERROR_CHECK(gpio_set_level(PIN_RS, 1)); /* Data mode */
    spi_transaction_t t = { .length = len * 8, .tx_buffer = data };
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));
}

static void lcd_write_data_byte(uint8_t data) {
    lcd_write_data(&data, 1);
}

/* --- DISPLAY DRIVER FUNCTIONS --- */
static void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t caset_data[4] = { x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF };
    lcd_write_cmd(CMD_CASET);
    lcd_write_data(caset_data, 4);

    uint8_t raset_data[4] = { y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF };
    lcd_write_cmd(CMD_RASET);
    lcd_write_data(raset_data, 4);

    lcd_write_cmd(CMD_RAMWR);
}

static void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour) {
    uint32_t total_pixels = w * h;
    lcd_set_window(x, y, x + w - 1, y + h - 1);

    /* High byte first for ST7796 */
    uint8_t color_hi = colour >> 8;
    uint8_t color_lo = colour & 0xFF;
    
    uint32_t chunk_px = (total_pixels < CHUNK_PIXELS) ? total_pixels : CHUNK_PIXELS;
    for (uint32_t i = 0; i < chunk_px; i++) {
        tx_buffer[i * 2]     = color_hi;
        tx_buffer[i * 2 + 1] = color_lo;
    }

    while (total_pixels > 0) {
        uint32_t px_to_send = (total_pixels > CHUNK_PIXELS) ? CHUNK_PIXELS : total_pixels;
        lcd_write_data(tx_buffer, px_to_send * 2);
        total_pixels -= px_to_send;
    }
}

static void lcd_init(void) {
    /* 1. Hardware Reset */
    ESP_ERROR_CHECK(gpio_set_level(PIN_RST, 0));
    vTaskDelay(pdMS_TO_TICKS(50));
    ESP_ERROR_CHECK(gpio_set_level(PIN_RST, 1));
    vTaskDelay(pdMS_TO_TICKS(150));

    /* 2. Standard Init Sequence */
    lcd_write_cmd(CMD_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150)); /* Mandatory wait */

    lcd_write_cmd(CMD_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(150)); /* Mandatory wait */

    lcd_write_cmd(CMD_MADCTL);
    lcd_write_data_byte(LCD_MADCTL_VAL);

    lcd_write_cmd(CMD_COLMOD);
    lcd_write_data_byte(0x55); /* 16-bit RGB565 */

    lcd_write_cmd(CMD_INVON); /* IPS Display Inversion ON - fixes inverted colors */
    
    lcd_write_cmd(CMD_DISPON);
    vTaskDelay(pdMS_TO_TICKS(50));
}

/* --- MAIN APPLICATION --- */
void app_main(void) {
    /* Init Backlight & Control Pins */
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_RS) | (1ULL << PIN_RST) | (1ULL << PIN_BK_LIGHT),
        .mode = GPIO_MODE_OUTPUT,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_set_level(PIN_BK_LIGHT, 1)); /* Backlight ON */

    /* Init SPI Bus */
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_SCK,
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = CHUNK_BYTES, /* Must be >= chunk size */
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = LCD_CLK_HZ,
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 7,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(LCD_HOST, &devcfg, &spi));

    /* Allocate SPI Tx Buffer (DMA capable) */
    tx_buffer = heap_caps_malloc(CHUNK_BYTES, MALLOC_CAP_DMA);
    assert(tx_buffer != NULL);

    /* Bring up LCD */
    lcd_init();

    /* Draw Three Bars */
    uint16_t bar_h = LCD_V_RES / 3;
    lcd_fill_rect(0, 0, LCD_H_RES, bar_h, COLOUR_RED);
    lcd_fill_rect(0, bar_h, LCD_H_RES, bar_h, COLOUR_GREEN);
    lcd_fill_rect(0, bar_h * 2, LCD_H_RES, LCD_V_RES - (bar_h * 2), COLOUR_BLUE);

    vTaskDelay(pdMS_TO_TICKS(3000)); /* Hold for 3 seconds */

    /* Cycle 5 Colours */
    const uint16_t cycle_colours[] = { COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE, COLOUR_WHITE, COLOUR_BLACK };
    uint8_t c_idx = 0;
    
    while (1) {
        lcd_fill_rect(0, 0, LCD_H_RES, LCD_V_RES, cycle_colours[c_idx]);
        c_idx = (c_idx + 1) % 5;
        vTaskDelay(pdMS_TO_TICKS(1000)); /* One second per colour */
    }
}