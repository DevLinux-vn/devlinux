#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "TOUCH";

#define LCD_HOST        SPI2_HOST

#define PIN_SCK         GPIO_NUM_12
#define PIN_MOSI        GPIO_NUM_11
#define PIN_MISO        GPIO_NUM_13
#define PIN_CS          GPIO_NUM_10
#define PIN_RS          GPIO_NUM_9
#define PIN_RST         GPIO_NUM_14
#define PIN_BK_LIGHT    GPIO_NUM_2

#define LCD_H_RES       (480U)
#define LCD_V_RES       (320U)

#define LCD_CLK_HZ      (20 * 1000 * 1000)
#define CHUNK_PIXELS    (1024U)
#define CHUNK_BYTES     (CHUNK_PIXELS * 2U)

/* ST7796 commands */
#define CMD_SWRESET     (0x01U)
#define CMD_SLPOUT      (0x11U)
#define CMD_INVON       (0x21U)
#define CMD_DISPON      (0x29U)
#define CMD_CASET       (0x2AU)
#define CMD_RASET       (0x2BU)
#define CMD_RAMWR       (0x2CU)
#define CMD_MADCTL      (0x36U)
#define CMD_COLMOD      (0x3AU)

/* MADCTL bits */
#define MADCTL_MY       (0x80U)
#define MADCTL_MX       (0x40U)
#define MADCTL_MV       (0x20U)
#define MADCTL_BGR      (0x08U)

#define COLOUR_RED      (0x001FU)
#define COLOUR_GREEN    (0x07E0U)
#define COLOUR_BLUE     (0xF800U)
#define COLOUR_WHITE    (0xFFFFU)
#define COLOUR_BLACK    (0x0000U)

/* ---------------- I2C / touch pin + register constants ---------------- */

#define PIN_TOUCH_SDA    GPIO_NUM_4
#define PIN_TOUCH_SCL    GPIO_NUM_5
#define PIN_TOUCH_RST    GPIO_NUM_6
#define PIN_TOUCH_INT    GPIO_NUM_7   /* wired, unused: this exercise polls */

#define TOUCH_I2C_PORT   (I2C_NUM_0)
#define TOUCH_I2C_ADDR   (0x38U)
#define I2C_CLK_HZ       (400000U)
#define I2C_TIMEOUT_MS   (100)        /* finite -- never -1 inside a poll loop */
#define I2C_GLITCH_IGNORE_CNT (7)

#define POLL_PERIOD_MS   (50U)        /* ~20 Hz */

#define RESET_LOW_MS     (10)
#define RESET_SETTLE_MS  (300)        /* FT6336U needs time to boot after reset */

/* FT6336U registers (see datasheet) */
#define REG_TD_STATUS    (0x02U)      /* low nibble = number of touch points */
#define REG_P1_XH        (0x03U)      /* then P1_XL, P1_YH, P1_YL follow */
#define REG_CHIP_ID      (0xA3U)
#define REG_VENDOR_ID    (0xA8U)

#define TD_STATUS_MASK        (0x0FU) /* number-of-points is low nibble */
#define TOUCH_COORD_HIGH_MASK (0x0FU) /* only low 4 bits of the high byte are coordinate */

#define P1_COORD_REG_COUNT (4U)       /* P1_XH, P1_XL, P1_YH, P1_YL */

#define SQUARE_SIDE      (20U)        /* ~20x20 px square centred on touch */

#define TOUCH_DEBUG_RAW  (0)   /* set to 1 to log raw (pre-mapping) x/y once */


static spi_device_handle_t lcd_spi;

static void lcd_gpio_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask =
            (1ULL << PIN_RS) |
            (1ULL << PIN_RST) |
            (1ULL << PIN_BK_LIGHT),

        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_set_level(PIN_RS, 1));
    ESP_ERROR_CHECK(gpio_set_level(PIN_RST, 1));
    ESP_ERROR_CHECK(gpio_set_level(PIN_BK_LIGHT, 1));
}

static void lcd_spi_init(void)
{
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_SCK,
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = CHUNK_BYTES,
    };

    ESP_ERROR_CHECK( spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = LCD_CLK_HZ,
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 7,
    };

    ESP_ERROR_CHECK( spi_bus_add_device(LCD_HOST, &devcfg, &lcd_spi));
}

static void lcd_write_cmd(uint8_t cmd)
{
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd,
    };
    ESP_ERROR_CHECK( gpio_set_level(PIN_RS, 0));
    ESP_ERROR_CHECK( spi_device_polling_transmit( lcd_spi, &t));
}

static void lcd_write_data(const uint8_t *data, size_t len)
{
    if (len == 0) {
        return;
    }

    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = data,
    };
    ESP_ERROR_CHECK(gpio_set_level(PIN_RS, 1));
    ESP_ERROR_CHECK(spi_device_polling_transmit( lcd_spi, &t));
}

static void lcd_hardware_reset(void)
{
    ESP_ERROR_CHECK(gpio_set_level(PIN_RST, 0));
    vTaskDelay(pdMS_TO_TICKS(20));
    ESP_ERROR_CHECK(gpio_set_level(PIN_RST, 1));
    vTaskDelay(pdMS_TO_TICKS(120));
}

static void lcd_init(void)
{
    lcd_hardware_reset();
    lcd_write_cmd(CMD_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));
    lcd_write_cmd(CMD_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(120));

    uint8_t madctl = MADCTL_MV | MADCTL_BGR;
    lcd_write_cmd(CMD_MADCTL);
    lcd_write_data(&madctl, 1);

    uint8_t colmod = 0x55;
    lcd_write_cmd(CMD_COLMOD);
    lcd_write_data(&colmod, 1);

    lcd_write_cmd(CMD_INVON);

    lcd_write_cmd(CMD_DISPON);

    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_LOGI(TAG, "ST7796 initialized");
}

static void lcd_set_window( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t data[4];
    lcd_write_cmd(CMD_CASET);

    data[0] = (uint8_t)(x0 >> 8);
    data[1] = (uint8_t)(x0 & 0xFF);

    data[2] = (uint8_t)(x1 >> 8);
    data[3] = (uint8_t)(x1 & 0xFF);

    lcd_write_data(data, sizeof(data));
    lcd_write_cmd(CMD_RASET);

    data[0] = (uint8_t)(y0 >> 8);
    data[1] = (uint8_t)(y0 & 0xFF);

    data[2] = (uint8_t)(y1 >> 8);
    data[3] = (uint8_t)(y1 & 0xFF);

    lcd_write_data(data, sizeof(data));

    lcd_write_cmd(CMD_RAMWR);
}

/* Signature kept exactly as Session 06: top-left corner + width/height. */
static void lcd_fill_rect( uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour)
{
    if (w == 0 || h == 0) {
        return;
    }
    if (x >= LCD_H_RES || y >= LCD_V_RES) {
        return;
    }
    if ((x + w) > LCD_H_RES) {
        w = LCD_H_RES - x;
    }
    if ((y + h) > LCD_V_RES) {
        h = LCD_V_RES - y;
    }
    lcd_set_window(
        x,
        y,
        x + w - 1,
        y + h - 1);

    static uint8_t buffer[CHUNK_BYTES];
    uint8_t high_byte = (uint8_t)(colour >> 8);
    uint8_t low_byte = (uint8_t)(colour & 0xFF);

    for (uint32_t i = 0; i < CHUNK_PIXELS; i++) {
        buffer[i * 2] = high_byte;
        buffer[i * 2 + 1] = low_byte;
    }
    uint32_t total_pixels = (uint32_t)w * h;

    while (total_pixels > 0) {
        uint32_t pixels_this_chunk = total_pixels > CHUNK_PIXELS ? CHUNK_PIXELS : total_pixels;
        size_t bytes_this_chunk = pixels_this_chunk * 2;
        lcd_write_data( buffer, bytes_this_chunk);
        total_pixels -= pixels_this_chunk;
    }
}

static void lcd_display_init(void)
{
    lcd_gpio_init();
    lcd_spi_init();
    lcd_init();
    /* Clear to black so the touch trail is visible against a clean background */
    lcd_fill_rect(0, 0, LCD_H_RES, LCD_V_RES, COLOUR_BLACK);
}

static inline void map_panel_to_screen(uint16_t raw_x, uint16_t raw_y,
                                        int *screen_x, int *screen_y)
{
    *screen_x = (int)raw_y;
    *screen_y = (int)raw_x;
}

static esp_err_t touch_read(i2c_master_dev_handle_t dev, uint8_t reg,
                             uint8_t *buf, size_t len)
{
    return i2c_master_transmit_receive(dev, &reg, 1U, buf, len, I2C_TIMEOUT_MS);
}

static void touch_hw_reset(void)
{
    gpio_config_t rst_cfg = {
        .pin_bit_mask = (1ULL << PIN_TOUCH_RST),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&rst_cfg));

    gpio_set_level(PIN_TOUCH_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(RESET_LOW_MS));
    gpio_set_level(PIN_TOUCH_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(RESET_SETTLE_MS));
}

static i2c_master_dev_handle_t touch_i2c_init(void)
{
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port                     = TOUCH_I2C_PORT,
        .sda_io_num                   = PIN_TOUCH_SDA,
        .scl_io_num                   = PIN_TOUCH_SCL,
        .clk_source                   = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt            = I2C_GLITCH_IGNORE_CNT,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = TOUCH_I2C_ADDR,
        .scl_speed_hz    = I2C_CLK_HZ,
    };
    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    return dev_handle;
}

static void touch_log_ids(i2c_master_dev_handle_t dev)
{
    uint8_t chip_id = 0;
    uint8_t vendor_id = 0;

    ESP_ERROR_CHECK(touch_read(dev, REG_CHIP_ID, &chip_id, 1U));
    ESP_ERROR_CHECK(touch_read(dev, REG_VENDOR_ID, &vendor_id, 1U));

    ESP_LOGI(TAG, "chip_id=0x%02X vendor_id=0x%02X", chip_id, vendor_id);

    if ((chip_id == 0x00U) || (chip_id == 0xFFU) ||
        (vendor_id == 0x00U) || (vendor_id == 0xFFU)) {
        ESP_LOGW(TAG, "ID read looks wrong (0x00/0xFF) -- check wiring/pull-ups");
    }
}

static int clampi(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void app_main(void)
{
    /* --- Session 06 display, unchanged orientation --- */
    lcd_display_init();

    /* --- Touch controller bring-up --- */
    touch_hw_reset();
    i2c_master_dev_handle_t touch_dev = touch_i2c_init();
    touch_log_ids(touch_dev);

    ESP_LOGI(TAG, "display ready, waiting for touch");

    while (1) {
        uint8_t td_status = 0;
        esp_err_t err = touch_read(touch_dev, REG_TD_STATUS, &td_status, 1U);

        if (err != ESP_OK) {
            ESP_LOGW(TAG, "TD_STATUS read failed: %s", esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS));
            continue;
        }

        uint8_t num_points = td_status & TD_STATUS_MASK;

        if (num_points >= 1U) {
            uint8_t coords[P1_COORD_REG_COUNT] = {0};
            err = touch_read(touch_dev, REG_P1_XH, coords, P1_COORD_REG_COUNT);

            if (err != ESP_OK) {
                ESP_LOGW(TAG, "coordinate read failed: %s", esp_err_to_name(err));
                vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS));
                continue;
            }

            uint16_t raw_x = (uint16_t)(((coords[0] & TOUCH_COORD_HIGH_MASK) << 8) | coords[1]);
            uint16_t raw_y = (uint16_t)(((coords[2] & TOUCH_COORD_HIGH_MASK) << 8) | coords[3]);

#if TOUCH_DEBUG_RAW
            ESP_LOGI(TAG, "raw @ x=%u y=%u", raw_x, raw_y);
#endif

            int x, y;
            map_panel_to_screen(raw_x, raw_y, &x, &y);

            int x0 = clampi(x - (int)(SQUARE_SIDE / 2), 0, (int)LCD_H_RES - (int)SQUARE_SIDE);
            int y0 = clampi(y - (int)(SQUARE_SIDE / 2), 0, (int)LCD_V_RES - (int)SQUARE_SIDE);

            ESP_LOGI(TAG, "touch @ x=%d y=%d", x, y);
            lcd_fill_rect((uint16_t)x0, (uint16_t)y0, SQUARE_SIDE, SQUARE_SIDE, COLOUR_WHITE);
        }

        vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS));
    }
}