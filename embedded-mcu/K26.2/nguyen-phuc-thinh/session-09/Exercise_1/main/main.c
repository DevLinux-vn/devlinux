/*
 * Display: ST7796U (SPI) - Landscape 480x320
 * ADC: Potentiometer on GPIO1 (ADC1_CHANNEL_0)
 * 
 * Attenuation Note: ADC_ATTEN_DB_12 provides the widest input range for the ESP32-S3, 
 * covering approximately 150 mV to 3100 mV of linear measurement.
 * 
 * Gap Note: The raw ADC values typically range from ~10 at minimum to ~4050 at maximum, 
 * failing to reach the theoretical 0 or 4095 limits[cite: 1]. This gap is expected 
 * ADC behavior, as the internal reference and physical limits prevent reading true 
 * 0V or 3.3V extremes[cite: 1].
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"

static const char *TAG = "ADC";

/* --- DISPLAY CONSTANTS --- */
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
#define LCD_MADCTL_VAL (0x28U) /* Landscape */

#define COLOUR_BLACK   (0x0000U)
#define COLOUR_WHITE   (0xFFFFU)
#define COLOUR_GREEN   (0x07E0U)

/* --- ADC & BAR CONSTANTS --- */
#define ADC_UNIT_USED    ADC_UNIT_1
#define ADC_CHANNEL_POT  ADC_CHANNEL_0 /* GPIO1 on ESP32-S3 */
#define ADC_ATTEN_USED   ADC_ATTEN_DB_12 
#define ADC_BITWIDTH     ADC_BITWIDTH_DEFAULT

#define SAMPLE_COUNT     (16U)
#define UPDATE_PERIOD_MS (200U)
#define VOLTAGE_MAX_MV   (3300)

#define BAR_X            (40U)
#define BAR_Y            (140U)
#define BAR_MAX_W        (400U)
#define BAR_H            (40U)

/* --- GLOBALS --- */
static spi_device_handle_t spi;
static uint8_t *tx_buffer;

/* --- DISPLAY DRIVERS --- */
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
    if (w == 0 || h == 0) return;
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

void app_main(void) {
    /* 1. Init Display */
    gpio_config_t io_out = {
        .pin_bit_mask = (1ULL<<PIN_RS)|(1ULL<<PIN_RST)|(1ULL<<PIN_BK_LIGHT),
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
    
    /* Draw outline for the bar graph */
    lcd_fill_rect(BAR_X - 2, BAR_Y - 2, BAR_MAX_W + 4, BAR_H + 4, COLOUR_WHITE);
    lcd_fill_rect(BAR_X, BAR_Y, BAR_MAX_W, BAR_H, COLOUR_BLACK);

    /* 2. Init ADC1 and Calibration */
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t unit_cfg = { .unit_id = ADC_UNIT_USED };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_USED,
        .bitwidth = ADC_BITWIDTH,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_POT, &chan_cfg));

    adc_cali_handle_t cali_handle = NULL;
    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id = ADC_UNIT_USED,
        .atten = ADC_ATTEN_USED,
        .bitwidth = ADC_BITWIDTH,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_cfg, &cali_handle));

    /* 3. Main Loop */
    uint32_t old_w = 0;

    while (1) {
        uint32_t sum = 0;
        int raw = 0;
        
        for (int i = 0; i < SAMPLE_COUNT; i++) {
            ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_POT, &raw));
            sum += raw;
        }
        
        int avg_raw = sum / SAMPLE_COUNT;
        int voltage_mv = 0;
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, avg_raw, &voltage_mv));

        ESP_LOGI(TAG, "raw= %4d  -> %4d mV", avg_raw, voltage_mv);

        /* Calculate new bar width safely */
        uint32_t new_w = (voltage_mv * BAR_MAX_W) / VOLTAGE_MAX_MV;
        if (new_w > BAR_MAX_W) new_w = BAR_MAX_W;

        /* Delta drawing to prevent flickering */
        if (new_w > old_w) {
            lcd_fill_rect(BAR_X + old_w, BAR_Y, new_w - old_w, BAR_H, COLOUR_GREEN);
        } else if (new_w < old_w) {
            lcd_fill_rect(BAR_X + new_w, BAR_Y, old_w - new_w, BAR_H, COLOUR_BLACK);
        }
        
        old_w = new_w;

        vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD_MS));
    }
}