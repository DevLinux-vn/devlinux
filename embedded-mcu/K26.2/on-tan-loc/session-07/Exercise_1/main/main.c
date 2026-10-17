#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"

#define TOUCH_I2C_ADDR   (0x38U)
#define PIN_TOUCH_SDA    GPIO_NUM_4
#define PIN_TOUCH_SCL    GPIO_NUM_5
#define PIN_TOUCH_RST    GPIO_NUM_6
#define PIN_TOUCH_INT    GPIO_NUM_7
#define I2C_CLK_HZ       (400000U)
#define I2C_TIMEOUT_MS   (100)   /* finite — never -1 inside a polling loop */
#define POLL_PERIOD_MS   (50U)
#define I2C_PORT_NUM       I2C_NUM_0

/* FT6336U registers */
#define REG_TD_STATUS    (0x02U) /* low nibble = number of touch points */
#define REG_P1_XH        (0x03U) /* then XL, YH, YL in the next three registers */
#define REG_CHIP_ID      (0xA3U)
#define REG_VENDOR_ID    (0xA8U)

#define TOUCH_COORD_MASK (0x0FU) /* only the low 4 bits of the high byte are coordinate */

static const char* TAG = "FT6336_DRIVER";

static i2c_master_bus_handle_t s_bus_handle = NULL;
static i2c_master_dev_handle_t s_dev_handle = NULL;

typedef struct {
    uint8_t touch_count;
    uint16_t x;
    uint16_t y;
    uint8_t event; // 0: Press Down, 1: Lift Up, 2: Contact, 3: No event
} ft6336_touch_data_t;

/* Read `len` bytes starting at `reg`, in one transaction */
static esp_err_t touch_read (i2c_master_dev_handle_t dev, uint8_t reg, uint8_t* buf, size_t len) {
    return i2c_master_transmit_receive(dev, &reg, 1U, 
                                       buf, len, I2C_TIMEOUT_MS);
}

static void ft6336_hardware_reset(void) {
    if (PIN_TOUCH_RST >= 0)
    {
        gpio_config_t rst_conf = {
            .pin_bit_mask = (1ULL << PIN_TOUCH_RST),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&rst_conf);

        ESP_LOGI(TAG, "Hardware Resetting FT6336U (Pin GPIO %d)...", PIN_TOUCH_RST);
        gpio_set_level(PIN_TOUCH_RST, 0); // Pull LOW
        vTaskDelay(pdMS_TO_TICKS(10));

        gpio_set_level(PIN_TOUCH_RST, 1); // Pull HIGH to wake up chip
        vTaskDelay(pdMS_TO_TICKS(100));   // Wait for internal firmware initialization
    }
}

static esp_err_t ft6336_init(void) {
    ft6336_hardware_reset();

    i2c_master_bus_config_t bus_cfg = {
        .i2c_port                     = I2C_PORT_NUM ,
        .sda_io_num                   = PIN_TOUCH_SDA,
        .scl_io_num                   = PIN_TOUCH_SCL,
        .clk_source                   = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt            = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &s_bus_handle));

    // Add FT6336U device to bus
    i2c_device_config_t touch_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = TOUCH_I2C_ADDR,
        .scl_speed_hz    = I2C_CLK_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(s_bus_handle, &touch_cfg, &s_dev_handle));

    // Read & verify Vendor ID and Chip ID registers
    uint8_t vendor_id = 0;
    uint8_t chip_id = 0;

    esp_err_t err = touch_read(s_dev_handle, REG_VENDOR_ID , &vendor_id, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to communicate with FT6336U! Error: %s", esp_err_to_name(err));
        return err;
    }

    err = touch_read(s_dev_handle, REG_CHIP_ID, &chip_id, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to check chip ID FT6336U! Error: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "TOUCH: vendor_id=0x%02X chip_id=0x%02X", vendor_id, chip_id);

    ESP_LOGI(TAG, "TOUCH: display ready, waiting for touch");

    return ESP_OK;
}

static esp_err_t ft6336_read_touch(ft6336_touch_data_t* touch) {
    uint8_t raw[6] = {0};

    // Read 5 consecutive bytes starting from register 0x02 (TD_STATUS)
    // [0] = TD_STATUS (Touch point count)
    // [1] = P1_XH (Event flag & X high 4 bits)
    // [2] = P1_XL (X low 8 bits)
    // [3] = P1_YH (Touch ID & Y high 4 bits)
    // [4] = P1_YL (Y low 8 bits)
    esp_err_t ret = touch_read(s_dev_handle, REG_TD_STATUS, raw, 5);
    if (ret != ESP_OK) {
        return ret;
    }

    touch->touch_count = raw[0] & 0x0F;
    if (touch->touch_count > 2) {
        touch->touch_count = 0;
    }

    if (touch->touch_count > 0) {
        touch->event = (raw[1] >> 6) & 0x03;
        touch->x = ((uint16_t) (raw[1] & 0x0F) << 8) | raw[2];
        touch->y = ((uint16_t) (raw[3] & 0x0F) << 8) | raw[4];
    } else {
        touch->x = 0;
        touch->y = 0;
        touch->event = 3; // No touch
    }

    return ESP_OK;
}

static void touch_task(void* pvParameters) {
    ft6336_touch_data_t touch;

    while (1) {
        // Polling Mode: Periodically wake up every 20ms
        if (ft6336_read_touch(&touch) == ESP_OK) {
            if (touch.touch_count > 0) {
                ESP_LOGI(TAG, "TOUCH: touch @ x = %4d | y = %4d", touch.x, touch.y);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS)); // 50 Hz
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "=== ESP32-S3 FT6336U I2C Capacitive Touch ===");
    ESP_ERROR_CHECK(ft6336_init());
    ESP_LOGI(TAG, "Mode: POLLING (Periodic 50ms check)");

    xTaskCreatePinnedToCore(touch_task, "touch_task", 4096, NULL, 5, NULL, 1);
}