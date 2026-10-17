/* 
 * Display: ST7796U (SPI) - Landscape 480x320
 * Touch: FT6336U (I2C) - Natively Portrait, mapped to Landscape
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

static const char *TAG = "TOUCH";

/* --- DISPLAY CONSTANTS (From Session 06) --- */
#define LCD_HOST       SPI2_HOST
#define PIN_SCK        GPIO_NUM_12
#define PIN_MOSI       GPIO_NUM_11
#define PIN_MISO       GPIO_NUM_13
#define PIN_CS         GPIO_NUM_10
#define PIN_RS         GPIO_NUM_9
#define PIN_RST        GPIO_NUM_14
#define PIN_BK_LIGHT   GPIO_NUM_2

#define LCD_H_RES      (480U)
#define LCD_V_RES      (320U)
#define LCD_CLK_HZ     (20 * 1000 * 1000)
#define CHUNK_PIXELS   (4800)
#define CHUNK_BYTES    (CHUNK_PIXELS * 2)

#define CMD_SWRESET    (0x01U)
#define CMD_SLPOUT     (0x11U)
#define CMD_INVON      (0x21U)
#define CMD_DISPON     (0x29U)
#define CMD_CASET      (0x2AU)
#define CMD_RASET      (0x2BU)
#define CMD_RAMWR      (0x2CU)
#define CMD_MADCTL     (0x36U)
#define CMD_COLMOD     (0x3AU)
#define LCD_MADCTL_VAL (0x28U) /* MV | BGR for Landscape */

#define COLOUR_WHITE   (0xFFFFU)
#define COLOUR_BLACK   (0x0000U)

/* --- TOUCH CONSTANTS --- */
#define TOUCH_I2C_ADDR   (0x38U)
#define PIN_TOUCH_SDA    GPIO_NUM_4
#define PIN_TOUCH_SCL    GPIO_NUM_5
#define PIN_TOUCH_RST    GPIO_NUM_6
#define PIN_TOUCH_INT    GPIO_NUM_7
#define I2C_CLK_HZ       (400000U)
#define I2C_TIMEOUT_MS   (100)
#define POLL_PERIOD_MS   (50U)

#define REG_TD_STATUS    (0x02U)
#define REG_P1_XH        (0x03U)
#define REG_CHIP_ID      (0xA3U)
#define REG_VENDOR_ID    (0xA8U)
#define TOUCH_COORD_MASK (0x0FU)

/* --- GLOBALS --- */
static spi_device_handle_t spi;
static uint8_t *tx_buffer;

/* --- DISPLAY DRIVER (Minimal) --- */
static void lcd_write_cmd(uint8_t cmd) {
    gpio_set_level(PIN_RS, 0);
    spi_transaction_t t = { .length = 8, .tx_buffer = &cmd };
    spi_device_polling_transmit(spi, &t);
}

static void lcd_write_data(const uint8_t *data, size_t len) {
    gpio_set_level(PIN_RS, 1);
    spi_transaction_t t = { .length = len * 8, .tx_buffer = data };
    spi_device_polling_transmit(spi, &t);
}

static void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t caset[4] = { x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF };
    lcd_write_cmd(CMD_CASET); lcd_write_data(caset, 4);
    uint8_t raset[4] = { y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF };
    lcd_write_cmd(CMD_RASET); lcd_write_data(raset, 4);
    lcd_write_cmd(CMD_RAMWR);
}

static void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour) {
    uint32_t total = w * h;
    lcd_set_window(x, y, x + w - 1, y + h - 1);
    uint8_t hi = colour >> 8, lo = colour & 0xFF;
    uint32_t chunk = (total < CHUNK_PIXELS) ? total : CHUNK_PIXELS;
    for (uint32_t i = 0; i < chunk; i++) {
        tx_buffer[i * 2] = hi; tx_buffer[i * 2 + 1] = lo;
    }
    while (total > 0) {
        uint32_t send = (total > CHUNK_PIXELS) ? CHUNK_PIXELS : total;
        lcd_write_data(tx_buffer, send * 2);
        total -= send;
    }
}

/* --- I2C TOUCH HELPER --- */
static esp_err_t touch_read(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t* buf, size_t len) {
    return i2c_master_transmit_receive(dev, &reg, 1U, buf, len, I2C_TIMEOUT_MS);
}

void app_main(void) {
    /* 1. Init SPI Display */
    gpio_config_t io_out = {
        .pin_bit_mask = (1ULL<<PIN_RS)|(1ULL<<PIN_RST)|(1ULL<<PIN_BK_LIGHT)|(1ULL<<PIN_TOUCH_RST),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_out);
    gpio_set_level(PIN_BK_LIGHT, 1);

    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_SCK, .mosi_io_num = PIN_MOSI, .miso_io_num = PIN_MISO,
        .quadwp_io_num = -1, .quadhd_io_num = -1, .max_transfer_sz = CHUNK_BYTES
    };
    spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = LCD_CLK_HZ, .mode = 0, .spics_io_num = PIN_CS, .queue_size = 7
    };
    spi_bus_add_device(LCD_HOST, &devcfg, &spi);
    tx_buffer = heap_caps_malloc(CHUNK_BYTES, MALLOC_CAP_DMA);

    gpio_set_level(PIN_RST, 0); vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(PIN_RST, 1); vTaskDelay(pdMS_TO_TICKS(150));
    
    lcd_write_cmd(CMD_SWRESET); vTaskDelay(pdMS_TO_TICKS(150));
    lcd_write_cmd(CMD_SLPOUT);  vTaskDelay(pdMS_TO_TICKS(150));
    lcd_write_cmd(CMD_MADCTL);  uint8_t m = LCD_MADCTL_VAL; lcd_write_data(&m, 1);
    lcd_write_cmd(CMD_COLMOD);  uint8_t c = 0x55; lcd_write_data(&c, 1);
    lcd_write_cmd(CMD_INVON);
    lcd_write_cmd(CMD_DISPON);  vTaskDelay(pdMS_TO_TICKS(50));

    lcd_fill_rect(0, 0, LCD_H_RES, LCD_V_RES, COLOUR_BLACK);

    /* 2. Reset Touch Controller */
    gpio_set_level(PIN_TOUCH_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(PIN_TOUCH_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    /* 3. Init I2C Bus */
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = PIN_TOUCH_SDA,
        .scl_io_num = PIN_TOUCH_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    i2c_device_config_t touch_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = TOUCH_I2C_ADDR,
        .scl_speed_hz = I2C_CLK_HZ,
    };
    i2c_master_dev_handle_t touch_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &touch_cfg, &touch_handle));

    /* 4. Verify ID Registers */
    uint8_t chip_id = 0, vendor_id = 0;
    if (touch_read(touch_handle, REG_CHIP_ID, &chip_id, 1) == ESP_OK && 
        touch_read(touch_handle, REG_VENDOR_ID, &vendor_id, 1) == ESP_OK) {
        ESP_LOGI(TAG, "chip_id=0x%02X vendor_id=0x%02X", chip_id, vendor_id);
        ESP_LOGI(TAG, "display ready, waiting for touch");
    } else {
        ESP_LOGE(TAG, "Failed to read ID. Wiring/pull-ups are wrong.");
        vTaskSuspend(NULL);
    }

    /* 5. Polling Loop */
    while (1) {
        uint8_t status = 0;
        esp_err_t err = touch_read(touch_handle, REG_TD_STATUS, &status, 1);
        
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "I2C read failed: %d", err);
        } else if ((status & 0x0F) > 0) {
            uint8_t coords[4];
            if (touch_read(touch_handle, REG_P1_XH, coords, 4) == ESP_OK) {
                uint16_t raw_x = ((coords[0] & TOUCH_COORD_MASK) << 8) | coords[1];
                uint16_t raw_y = ((coords[2] & TOUCH_COORD_MASK) << 8) | coords[3];

                /* EMPIRICAL MAPPING: Touch panel is native portrait.
                 * Swap X and Y, and invert Y to align with landscape display. 
                 * Range observed: X(0-319), Y(0-479). */
                int16_t mapped_x = raw_y;
                int16_t mapped_y = LCD_V_RES - 1 - raw_x;

                ESP_LOGI(TAG, "touch @ x=%d y=%d", mapped_x, mapped_y);

                int16_t rect_x = mapped_x - 10;
                int16_t rect_y = mapped_y - 10;
                
                if (rect_x < 0) rect_x = 0;
                if (rect_y < 0) rect_y = 0;
                if (rect_x + 20 > LCD_H_RES) rect_x = LCD_H_RES - 20;
                if (rect_y + 20 > LCD_V_RES) rect_y = LCD_V_RES - 20;

                lcd_fill_rect(rect_x, rect_y, 20, 20, COLOUR_WHITE);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS));
    }
}