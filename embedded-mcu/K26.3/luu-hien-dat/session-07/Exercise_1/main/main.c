#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ST7796U display wiring from Session 06. */
#define LCD_SPI_HOST                 SPI2_HOST
#define LCD_PIN_SCLK                 GPIO_NUM_12
#define LCD_PIN_MOSI                 GPIO_NUM_11
#define LCD_PIN_MISO                 GPIO_NUM_13
#define LCD_PIN_CS                   GPIO_NUM_10
#define LCD_PIN_DC                   GPIO_NUM_9
#define LCD_PIN_RESET                GPIO_NUM_14
#define LCD_PIN_BACKLIGHT            GPIO_NUM_2

#define LCD_WIDTH                    480U
#define LCD_HEIGHT                   320U
#define LCD_SPI_CLOCK_HZ             (20 * 1000 * 1000)
#define LCD_PIXEL_CHUNK_SIZE         320U
#define LCD_MAX_TRANSFER_SIZE        (LCD_PIXEL_CHUNK_SIZE * 2U)

#define LCD_CMD_SWRESET              0x01U
#define LCD_CMD_SLPOUT               0x11U
#define LCD_CMD_INVON                0x21U
#define LCD_CMD_DISPON               0x29U
#define LCD_CMD_CASET                0x2AU
#define LCD_CMD_RASET                0x2BU
#define LCD_CMD_RAMWR                0x2CU
#define LCD_CMD_MADCTL               0x36U
#define LCD_CMD_COLMOD               0x3AU
#define LCD_MADCTL_LANDSCAPE_BGR     0xE8U
#define LCD_COLMOD_RGB565            0x55U
#define LCD_COLOR_BLACK              0x0000U

/* FT6336U touch wiring from the Session 07 assignment. */
#define TOUCH_I2C_PORT                I2C_NUM_0
#define TOUCH_PIN_SDA                 GPIO_NUM_4
#define TOUCH_PIN_SCL                 GPIO_NUM_5
#define TOUCH_PIN_RESET               GPIO_NUM_6
#define TOUCH_PIN_INTERRUPT           GPIO_NUM_7 /* Connected as required; polling does not use it. */
#define TOUCH_I2C_ADDRESS             0x38U
#define TOUCH_I2C_FREQUENCY_HZ        400000U
#define TOUCH_I2C_TIMEOUT_MS          100
#define TOUCH_POLL_INTERVAL_MS        50U

#define FT6336_REG_TOUCH_COUNT        0x02U
#define FT6336_REG_POINT1_X_HIGH      0x03U
#define FT6336_REG_CHIP_ID            0xA3U
#define FT6336_REG_VENDOR_ID          0xA8U
#define FT6336_REG_CHIP_MODEL_MID     0x9FU
#define FT6336_REG_CHIP_MODEL_LOW     0xA0U
#define FT6336_TOUCH_COUNT_MASK       0x0FU
#define FT6336_COORDINATE_HIGH_MASK   0x0FU
#define FT6336_EXPECTED_CHIP_ID       0x64U
#define FT6336_EXPECTED_VENDOR_ID     0x11U
#define FT6336_EXPECTED_MODEL_MID     0x26U
#define FT6336_EXPECTED_MODEL_LOW     0x02U

#define TOUCH_MARKER_SIZE             20
#define TOUCH_MARKER_HALF_SIZE        (TOUCH_MARKER_SIZE / 2)

/*
 * The panel's native touch axes are expected to be portrait while the LCD is
 * landscape. This provisional mapping swaps X/Y. Validate by touching all
 * four screen edges, then record the measured raw ranges and any needed axis
 * inversions here. No hardware edge measurements are available at source time.
 * If the marker moves in the wrong direction, adjust the transform below.
 */
#define TOUCH_SWAP_AXES               1
#define TOUCH_INVERT_DISPLAY_X        0
#define TOUCH_INVERT_DISPLAY_Y        0
#define TOUCH_RAW_X_MIN               0U
#define TOUCH_RAW_X_MAX               319U
#define TOUCH_RAW_Y_MIN               0U
#define TOUCH_RAW_Y_MAX               479U

static const char *TAG = "ft6336_touch_trail";
static spi_device_handle_t s_lcd_spi;
static i2c_master_bus_handle_t s_touch_bus;
static i2c_master_dev_handle_t s_touch_device;

static void lcd_write_command(uint8_t command)
{
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_DC, 0));
    const spi_transaction_t transaction = {
        .length = 8,
        .tx_buffer = &command,
    };
    ESP_ERROR_CHECK(spi_device_polling_transmit(s_lcd_spi, &transaction));
}

static void lcd_write_data(const uint8_t *data, size_t length)
{
    if (length == 0U) {
        return;
    }

    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_DC, 1));
    const spi_transaction_t transaction = {
        .length = length * 8U,
        .tx_buffer = data,
    };
    ESP_ERROR_CHECK(spi_device_polling_transmit(s_lcd_spi, &transaction));
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

static void lcd_initialize(void)
{
    const gpio_config_t gpio_output_config = {
        .pin_bit_mask = (1ULL << LCD_PIN_DC) |
                        (1ULL << LCD_PIN_RESET) |
                        (1ULL << LCD_PIN_BACKLIGHT),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_output_config));
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_BACKLIGHT, 1));
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_DC, 1));
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_RESET, 1));

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
    ESP_ERROR_CHECK(spi_bus_add_device(LCD_SPI_HOST, &device_config, &s_lcd_spi));

    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_RESET, 0));
    vTaskDelay(pdMS_TO_TICKS(20));
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_RESET, 1));
    vTaskDelay(pdMS_TO_TICKS(120));

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

    lcd_fill_rect(0, 0, (int)LCD_WIDTH, (int)LCD_HEIGHT, LCD_COLOR_BLACK);
}

static void touch_reset(void)
{
    const gpio_config_t reset_config = {
        .pin_bit_mask = 1ULL << TOUCH_PIN_RESET,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&reset_config));
    ESP_ERROR_CHECK(gpio_set_level(TOUCH_PIN_RESET, 0));
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_ERROR_CHECK(gpio_set_level(TOUCH_PIN_RESET, 1));
    vTaskDelay(pdMS_TO_TICKS(100));
}

static void touch_i2c_initialize(void)
{
    const i2c_master_bus_config_t bus_config = {
        .i2c_port = TOUCH_I2C_PORT,
        .sda_io_num = TOUCH_PIN_SDA,
        .scl_io_num = TOUCH_PIN_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &s_touch_bus));

    const i2c_device_config_t device_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = TOUCH_I2C_ADDRESS,
        .scl_speed_hz = TOUCH_I2C_FREQUENCY_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(s_touch_bus, &device_config, &s_touch_device));
}

static esp_err_t touch_read_register(uint8_t reg, uint8_t *value)
{
    return i2c_master_transmit_receive(s_touch_device, &reg, 1, value, 1, TOUCH_I2C_TIMEOUT_MS);
}

static esp_err_t touch_verify_controller(void)
{
    uint8_t chip_id = 0U;
    uint8_t vendor_id = 0U;
    uint8_t model_mid = 0U;
    uint8_t model_low = 0U;

    esp_err_t err = touch_read_register(FT6336_REG_CHIP_ID, &chip_id);
    if (err != ESP_OK) {
        return err;
    }
    err = touch_read_register(FT6336_REG_VENDOR_ID, &vendor_id);
    if (err != ESP_OK) {
        return err;
    }
    err = touch_read_register(FT6336_REG_CHIP_MODEL_MID, &model_mid);
    if (err != ESP_OK) {
        return err;
    }
    err = touch_read_register(FT6336_REG_CHIP_MODEL_LOW, &model_low);
    if (err != ESP_OK) {
        return err;
    }

    ESP_LOGI(TAG, "FT6336 IDs: chip(0x%02X)=0x%02X, vendor(0x%02X)=0x%02X",
             FT6336_REG_CHIP_ID, chip_id, FT6336_REG_VENDOR_ID, vendor_id);
    ESP_LOGI(TAG, "Model bytes: 0x9F=0x%02X, 0xA0=0x%02X", model_mid, model_low);

    if (chip_id != FT6336_EXPECTED_CHIP_ID || vendor_id != FT6336_EXPECTED_VENDOR_ID ||
        model_mid != FT6336_EXPECTED_MODEL_MID || model_low != FT6336_EXPECTED_MODEL_LOW) {
        ESP_LOGE(TAG, "FT6336U ID mismatch; expected A3=64, A8=11, 9F=26, A0=02");
        return ESP_ERR_INVALID_RESPONSE;
    }

    return ESP_OK;
}

static esp_err_t touch_read_first_point(bool *is_touched, uint16_t *raw_x, uint16_t *raw_y)
{
    uint8_t reg = FT6336_REG_TOUCH_COUNT;
    uint8_t data[5] = {0U};
    const esp_err_t err = i2c_master_transmit_receive(
        s_touch_device, &reg, 1, data, sizeof(data), TOUCH_I2C_TIMEOUT_MS);
    if (err != ESP_OK) {
        return err;
    }

    const uint8_t touch_count = data[0] & FT6336_TOUCH_COUNT_MASK;
    *is_touched = (touch_count > 0U);
    if (!*is_touched) {
        return ESP_OK;
    }

    *raw_x = ((uint16_t)(data[1] & FT6336_COORDINATE_HIGH_MASK) << 8) | data[2];
    *raw_y = ((uint16_t)(data[3] & FT6336_COORDINATE_HIGH_MASK) << 8) | data[4];
    return ESP_OK;
}

static uint16_t clamp_u16(uint16_t value, uint16_t maximum)
{
    return (value > maximum) ? maximum : value;
}

static uint16_t scale_touch_axis(uint16_t value, uint16_t raw_min, uint16_t raw_max,
                                 uint16_t display_size)
{
    if (display_size <= 1U || raw_max <= raw_min) {
        return 0U;
    }
    if (value <= raw_min) {
        return 0U;
    }
    if (value >= raw_max) {
        return (uint16_t)(display_size - 1U);
    }

    const uint32_t raw_span = (uint32_t)raw_max - raw_min;
    const uint32_t display_span = (uint32_t)display_size - 1U;
    return (uint16_t)(((uint32_t)(value - raw_min) * display_span) / raw_span);
}

static void touch_map_to_display(uint16_t raw_x, uint16_t raw_y,
                                 uint16_t *display_x, uint16_t *display_y)
{
    uint16_t x;
    uint16_t y;

#if TOUCH_SWAP_AXES
    x = scale_touch_axis(raw_y, TOUCH_RAW_Y_MIN, TOUCH_RAW_Y_MAX, (uint16_t)LCD_WIDTH);
    y = scale_touch_axis(raw_x, TOUCH_RAW_X_MIN, TOUCH_RAW_X_MAX, (uint16_t)LCD_HEIGHT);
#else
    x = scale_touch_axis(raw_x, TOUCH_RAW_X_MIN, TOUCH_RAW_X_MAX, (uint16_t)LCD_WIDTH);
    y = scale_touch_axis(raw_y, TOUCH_RAW_Y_MIN, TOUCH_RAW_Y_MAX, (uint16_t)LCD_HEIGHT);
#endif

#if TOUCH_INVERT_DISPLAY_X
    x = (uint16_t)(LCD_WIDTH - 1U - x);
#endif
#if TOUCH_INVERT_DISPLAY_Y
    y = (uint16_t)(LCD_HEIGHT - 1U - y);
#endif

    *display_x = clamp_u16(x, (uint16_t)(LCD_WIDTH - 1U));
    *display_y = clamp_u16(y, (uint16_t)(LCD_HEIGHT - 1U));
}

static void draw_touch_marker(uint16_t center_x, uint16_t center_y)
{
    int x = (int)center_x - TOUCH_MARKER_HALF_SIZE;
    int y = (int)center_y - TOUCH_MARKER_HALF_SIZE;
    const int max_x = (int)LCD_WIDTH - TOUCH_MARKER_SIZE;
    const int max_y = (int)LCD_HEIGHT - TOUCH_MARKER_SIZE;

    if (x < 0) {
        x = 0;
    } else if (x > max_x) {
        x = max_x;
    }
    if (y < 0) {
        y = 0;
    } else if (y > max_y) {
        y = max_y;
    }

    lcd_fill_rect(x, y, TOUCH_MARKER_SIZE, TOUCH_MARKER_SIZE, 0xFFFFU);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initialising ST7796U display and FT6336U touch controller");
    lcd_initialize();

    /* Reset the controller before setting up and accessing it on I2C. */
    touch_reset();
    touch_i2c_initialize();
    const esp_err_t id_err = touch_verify_controller();
    if (id_err != ESP_OK) {
        ESP_LOGE(TAG, "Touch controller verification failed: %s", esp_err_to_name(id_err));
        return;
    }

    while (true) {
        bool is_touched = false;
        uint16_t raw_x = 0U;
        uint16_t raw_y = 0U;
        const esp_err_t err = touch_read_first_point(&is_touched, &raw_x, &raw_y);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Touch read failed: %s; retrying next cycle", esp_err_to_name(err));
        } else if (is_touched) {
            uint16_t display_x = 0U;
            uint16_t display_y = 0U;
            touch_map_to_display(raw_x, raw_y, &display_x, &display_y);
            ESP_LOGI(TAG, "Touch raw=(%u,%u), display=(%u,%u)",
                     (unsigned int)raw_x, (unsigned int)raw_y,
                     (unsigned int)display_x, (unsigned int)display_y);
            draw_touch_marker(display_x, display_y);
        }

        vTaskDelay(pdMS_TO_TICKS(TOUCH_POLL_INTERVAL_MS));
    }
}
