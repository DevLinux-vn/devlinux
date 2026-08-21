#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/gptimer.h"
#include "esp_rom_sys.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define I2C_MASTER_SCL_IO           1U
#define I2C_MASTER_SDA_IO           2U
#define I2C_MASTER_FREQ_HZ          400000U
#define LCD_ADDR                    0x3CU

#define DHT11_PIN                   4U
#define TIMER_RESOLUTION_HZ         1000000U /* 1MHz, 1 tick = 1us */
#define ALARM_PERIOD_US             5000000U /* 5s cycle time */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static esp_err_t initI2c(void);
static esp_err_t initTimerInterrupt(void);
static esp_err_t clearLcd(void);
static esp_err_t sendLcdCommand(uint8_t cmd);
static esp_err_t sendLcdData(uint8_t data);
static esp_err_t setCursor(uint8_t col, uint8_t page);
static esp_err_t drawChar(char c);
static esp_err_t drawString(const char *str);
static esp_err_t waitPinState(uint32_t targetLevel, uint32_t timeoutUs);
static esp_err_t readDht11(uint8_t *temperature, uint8_t *humidity);
static bool timerIsrCallback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *userCtx);
void app_main(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static volatile uint8_t s_readSensorFlag = 0U;
static i2c_master_bus_handle_t s_i2cBusHandle = NULL;
static i2c_master_dev_handle_t s_lcdDevHandle = NULL;

/* Bảng mã ASCII chuẩn 5x8 (Từ ký tự Space ' ' đến '~') */
static const uint8_t s_font5x8[95][5] = {
    {0x00U, 0x00U, 0x00U, 0x00U, 0x00U}, /* 32: Space */
    {0x00U, 0x00U, 0x4FU, 0x00U, 0x00U}, /* 33: ! */
    {0x00U, 0x07U, 0x00U, 0x07U, 0x00U}, /* 34: " */
    {0x14U, 0x7FU, 0x14U, 0x7FU, 0x14U}, /* 35: # */
    {0x24U, 0x2AU, 0x7FU, 0x2AU, 0x12U}, /* 36: $ */
    {0x23U, 0x13U, 0x08U, 0x64U, 0x62U}, /* 37: % */
    {0x36U, 0x49U, 0x55U, 0x22U, 0x50U}, /* 38: & */
    {0x00U, 0x05U, 0x03U, 0x00U, 0x00U}, /* 39: ' */
    {0x00U, 0x1CU, 0x22U, 0x41U, 0x00U}, /* 40: ( */
    {0x00U, 0x41U, 0x22U, 0x1CU, 0x00U}, /* 41: ) */
    {0x14U, 0x08U, 0x3EU, 0x08U, 0x14U}, /* 42: * */
    {0x08U, 0x08U, 0x3EU, 0x08U, 0x08U}, /* 43: + */
    {0x00U, 0x50U, 0x30U, 0x00U, 0x00U}, /* 44: , */
    {0x08U, 0x08U, 0x08U, 0x08U, 0x08U}, /* 45: - */
    {0x00U, 0x60U, 0x60U, 0x00U, 0x00U}, /* 46: . */
    {0x20U, 0x10U, 0x08U, 0x04U, 0x02U}, /* 47: / */
    {0x3EU, 0x51U, 0x49U, 0x45U, 0x3EU}, /* 48: 0 */
    {0x00U, 0x42U, 0x7FU, 0x40U, 0x00U}, /* 49: 1 */
    {0x42U, 0x61U, 0x51U, 0x49U, 0x46U}, /* 50: 2 */
    {0x21U, 0x41U, 0x45U, 0x4BU, 0x31U}, /* 51: 3 */
    {0x18U, 0x14U, 0x12U, 0x7FU, 0x10U}, /* 52: 4 */
    {0x27U, 0x45U, 0x45U, 0x45U, 0x39U}, /* 53: 5 */
    {0x3CU, 0x4AU, 0x49U, 0x49U, 0x30U}, /* 54: 6 */
    {0x01U, 0x71U, 0x09U, 0x05U, 0x03U}, /* 55: 7 */
    {0x36U, 0x49U, 0x49U, 0x49U, 0x36U}, /* 56: 8 */
    {0x06U, 0x49U, 0x49U, 0x29U, 0x1EU}, /* 57: 9 */
    {0x00U, 0x36U, 0x36U, 0x00U, 0x00U}, /* 58: : */
    {0x00U, 0x56U, 0x36U, 0x00U, 0x00U}, /* 59: ; */
    {0x08U, 0x14U, 0x22U, 0x41U, 0x00U}, /* 60: < */
    {0x14U, 0x14U, 0x14U, 0x14U, 0x14U}, /* 61: = */
    {0x00U, 0x41U, 0x22U, 0x14U, 0x08U}, /* 62: > */
    {0x02U, 0x01U, 0x51U, 0x09U, 0x06U}, /* 63: ? */
    {0x32U, 0x49U, 0x79U, 0x41U, 0x3EU}, /* 64: @ */
    {0x7EU, 0x11U, 0x11U, 0x11U, 0x7EU}, /* 65: A */
    {0x7FU, 0x49U, 0x49U, 0x49U, 0x36U}, /* 66: B */
    {0x3EU, 0x41U, 0x41U, 0x41U, 0x22U}, /* 67: C */
    {0x7FU, 0x41U, 0x41U, 0x22U, 0x1CU}, /* 68: D */
    {0x7FU, 0x49U, 0x49U, 0x49U, 0x41U}, /* 69: E */
    {0x7FU, 0x09U, 0x09U, 0x09U, 0x01U}, /* 70: F */
    {0x3EU, 0x41U, 0x49U, 0x49U, 0x7AU}, /* 71: G */
    {0x7FU, 0x08U, 0x08U, 0x08U, 0x7FU}, /* 72: H */
    {0x00U, 0x41U, 0x7FU, 0x41U, 0x00U}, /* 73: I */
    {0x20U, 0x40U, 0x41U, 0x3FU, 0x01U}, /* 74: J */
    {0x7FU, 0x08U, 0x14U, 0x22U, 0x41U}, /* 75: K */
    {0x7FU, 0x40U, 0x40U, 0x40U, 0x40U}, /* 76: L */
    {0x7FU, 0x02U, 0x0CU, 0x02U, 0x7FU}, /* 77: M */
    {0x7FU, 0x04U, 0x08U, 0x10U, 0x7FU}, /* 78: N */
    {0x3EU, 0x41U, 0x41U, 0x41U, 0x3EU}, /* 79: O */
    {0x7FU, 0x09U, 0x09U, 0x09U, 0x06U}, /* 80: P */
    {0x3EU, 0x41U, 0x51U, 0x21U, 0x5EU}, /* 81: Q */
    {0x7FU, 0x09U, 0x19U, 0x29U, 0x46U}, /* 82: R */
    {0x46U, 0x49U, 0x49U, 0x49U, 0x31U}, /* 83: S */
    {0x01U, 0x01U, 0x7FU, 0x01U, 0x01U}, /* 84: T */
    {0x3FU, 0x40U, 0x40U, 0x40U, 0x3FU}, /* 85: U */
    {0x1FU, 0x20U, 0x40U, 0x20U, 0x1FU}, /* 86: V */
    {0x3FU, 0x40U, 0x38U, 0x40U, 0x3FU}, /* 87: W */
    {0x63U, 0x14U, 0x08U, 0x14U, 0x63U}, /* 88: X */
    {0x07U, 0x08U, 0x70U, 0x08U, 0x07U}, /* 89: Y */
    {0x61U, 0x51U, 0x49U, 0x45U, 0x43U}, /* 90: Z */
    {0x00U, 0x7FU, 0x41U, 0x41U, 0x00U}, /* 91: [ */
    {0x02U, 0x04U, 0x08U, 0x10U, 0x20U}, /* 92: \ */
    {0x00U, 0x41U, 0x41U, 0x7FU, 0x00U}, /* 93: ] */
    {0x04U, 0x02U, 0x01U, 0x02U, 0x04U}, /* 94: ^ */
    {0x40U, 0x40U, 0x40U, 0x40U, 0x40U}, /* 95: _ */
    {0x00U, 0x01U, 0x02U, 0x04U, 0x00U}, /* 96: ` */
    {0x20U, 0x54U, 0x54U, 0x54U, 0x78U}, /* 97: a */
    {0x7FU, 0x48U, 0x44U, 0x44U, 0x38U}, /* 98: b */
    {0x38U, 0x44U, 0x44U, 0x44U, 0x20U}, /* 99: c */
    {0x38U, 0x44U, 0x44U, 0x48U, 0x7FU}, /* 100: d */
    {0x38U, 0x54U, 0x54U, 0x54U, 0x18U}, /* 101: e */
    {0x08U, 0x7EU, 0x09U, 0x01U, 0x02U}, /* 102: f */
    {0x0CU, 0x52U, 0x52U, 0x52U, 0x3EU}, /* 103: g */
    {0x7FU, 0x08U, 0x04U, 0x04U, 0x78U}, /* 104: h */
    {0x00U, 0x44U, 0x7DU, 0x40U, 0x00U}, /* 105: i */
    {0x20U, 0x40U, 0x44U, 0x3DU, 0x00U}, /* 106: j */
    {0x7FU, 0x10U, 0x28U, 0x44U, 0x00U}, /* 107: k */
    {0x00U, 0x41U, 0x7FU, 0x40U, 0x00U}, /* 108: l */
    {0x7CU, 0x04U, 0x18U, 0x04U, 0x78U}, /* 109: m */
    {0x7CU, 0x08U, 0x04U, 0x04U, 0x78U}, /* 110: n */
    {0x38U, 0x44U, 0x44U, 0x44U, 0x38U}, /* 111: o */
    {0x7CU, 0x14U, 0x14U, 0x14U, 0x08U}, /* 112: p */
    {0x08U, 0x14U, 0x14U, 0x18U, 0x7CU}, /* 113: q */
    {0x7CU, 0x08U, 0x04U, 0x04U, 0x08U}, /* 114: r */
    {0x48U, 0x54U, 0x54U, 0x54U, 0x20U}, /* 115: s */
    {0x04U, 0x3FU, 0x44U, 0x40U, 0x20U}, /* 116: t */
    {0x3CU, 0x40U, 0x40U, 0x20U, 0x7CU}, /* 117: u */
    {0x1CU, 0x20U, 0x40U, 0x20U, 0x1CU}, /* 118: v */
    {0x3CU, 0x40U, 0x30U, 0x40U, 0x3CU}, /* 119: w */
    {0x44U, 0x28U, 0x10U, 0x28U, 0x44U}, /* 120: x */
    {0x0CU, 0x50U, 0x50U, 0x50U, 0x3CU}, /* 121: y */
    {0x44U, 0x64U, 0x54U, 0x4CU, 0x44U}, /* 122: z */
    {0x00U, 0x08U, 0x36U, 0x41U, 0x00U}, /* 123: { */
    {0x00U, 0x00U, 0x7FU, 0x00U, 0x00U}, /* 124: | */
    {0x00U, 0x41U, 0x36U, 0x08U, 0x00U}, /* 125: } */
    {0x10U, 0x08U, 0x18U, 0x10U, 0x08U}  /* 126: ~ */
};

/*******************************************************************************
 * Code
 ******************************************************************************/
static esp_err_t initI2c(void)
{
    esp_err_t ret = ESP_OK;
    i2c_master_bus_config_t busConf = {
        .i2c_port = -1, 
        .sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO,
        .scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7U,
        .intr_priority = 0,
        .trans_queue_depth = 0U,
        .flags = {
            .enable_internal_pullup = 1U
        }
    };
    i2c_device_config_t devConf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LCD_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
        .scl_wait_us = 0U,
        .flags = {
            .disable_ack_check = 0U
        }
    };

    ret = i2c_new_master_bus(&busConf, &s_i2cBusHandle);
    if (ret == ESP_OK)
    {
        ret = i2c_master_bus_add_device(s_i2cBusHandle, &devConf, &s_lcdDevHandle);
    }

    return ret;
}

static bool IRAM_ATTR timerIsrCallback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *userCtx)
{
    bool highTaskAwoken = false;

    (void)timer;
    (void)edata;
    (void)userCtx;

    s_readSensorFlag = 1U;

    return highTaskAwoken;
}

static esp_err_t initTimerInterrupt(void)
{
    esp_err_t ret = ESP_OK;
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timerConfig = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION_HZ,
        .intr_priority = 0
    };
    gptimer_alarm_config_t alarmConfig = {
        .alarm_count = ALARM_PERIOD_US,
        .reload_count = 0U,
        .flags = {
            .auto_reload_on_alarm = 1U
        }
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timerIsrCallback
    };

    ret = gptimer_new_timer(&timerConfig, &gptimer);
    if (ret == ESP_OK)
    {
        ret = gptimer_register_event_callbacks(gptimer, &cbs, NULL);
        if (ret == ESP_OK)
        {
            ret = gptimer_enable(gptimer);
            if (ret == ESP_OK)
            {
                ret = gptimer_set_alarm_action(gptimer, &alarmConfig);
                if (ret == ESP_OK)
                {
                    ret = gptimer_start(gptimer);
                }
            }
        }
    }

    return ret;
}

static esp_err_t sendLcdCommand(uint8_t cmd)
{
    esp_err_t ret = ESP_FAIL;
    uint8_t buffer[2] = {0x00U, 0x00U};

    if (s_lcdDevHandle != NULL)
    {
        buffer[0] = 0x00U;
        buffer[1] = cmd;
        ret = i2c_master_transmit(s_lcdDevHandle, buffer, 2U, -1);
    }

    return ret;
}

static esp_err_t sendLcdData(uint8_t data)
{
    esp_err_t ret = ESP_FAIL;
    uint8_t buffer[2] = {0x40U, 0x00U};

    if (s_lcdDevHandle != NULL)
    {
        buffer[0] = 0x40U;
        buffer[1] = data;
        ret = i2c_master_transmit(s_lcdDevHandle, buffer, 2U, -1);
    }

    return ret;
}

static esp_err_t clearLcd(void)
{
    esp_err_t ret = ESP_OK;
    uint8_t page = 0U;
    uint8_t col = 0U;

    while ((page < 8U) && (ret == ESP_OK))
    {
        ret = sendLcdCommand((uint8_t)(0xB0U + page)); 
        if (ret == ESP_OK) { ret = sendLcdCommand(0x00U); } 
        if (ret == ESP_OK) { ret = sendLcdCommand(0x10U); } 

        if (ret == ESP_OK)
        {
            col = 0U;
            while ((col < 128U) && (ret == ESP_OK))
            {
                ret = sendLcdData(0x00U); 
                col++;
            }
        }
        page++;
    }

    return ret;
}

static esp_err_t setCursor(uint8_t col, uint8_t page)
{
    esp_err_t ret = ESP_OK;

    if ((page < 8U) && (col < 128U))
    {
        ret = sendLcdCommand((uint8_t)(0xB0U + page));
        if (ret == ESP_OK)
        {
            ret = sendLcdCommand((uint8_t)(col & 0x0FU));
        }
        if (ret == ESP_OK)
        {
            ret = sendLcdCommand((uint8_t)(0x10U | (col >> 4U)));
        }
    }
    else
    {
        ret = ESP_FAIL;
    }

    return ret;
}

static esp_err_t drawChar(char c)
{
    esp_err_t ret = ESP_OK;
    uint8_t i = 0U;
    uint8_t charIdx = 0U;

    if ((c >= ' ') && (c <= '~'))
    {
        charIdx = (uint8_t)(c - ' ');
        while ((i < 5U) && (ret == ESP_OK))
        {
            ret = sendLcdData(s_font5x8[charIdx][i]);
            i++;
        }
        
        if (ret == ESP_OK)
        {
            ret = sendLcdData(0x00U); /* Space between characters */
        }
    }
    else
    {
        ret = ESP_ERR_INVALID_ARG;
    }

    return ret;
}

static esp_err_t drawString(const char *str)
{
    esp_err_t ret = ESP_OK;
    uint32_t i = 0U;

    if (str != NULL)
    {
        while ((str[i] != '\0') && (ret == ESP_OK))
        {
            ret = drawChar(str[i]);
            i++;
        }
    }
    else
    {
        ret = ESP_ERR_INVALID_ARG;
    }

    return ret;
}

static esp_err_t waitPinState(uint32_t targetLevel, uint32_t timeoutUs)
{
    esp_err_t ret = ESP_OK;
    uint32_t count = 0U;
    uint32_t level = 0U;

    level = (uint32_t)gpio_get_level(DHT11_PIN);
    
    while ((level != targetLevel) && (count < timeoutUs))
    {
        esp_rom_delay_us(1U);
        count++;
        level = (uint32_t)gpio_get_level(DHT11_PIN);
    }

    if (count >= timeoutUs)
    {
        ret = ESP_FAIL;
    }

    return ret;
}

static esp_err_t readDht11(uint8_t *temperature, uint8_t *humidity)
{
    esp_err_t ret = ESP_OK;
    uint8_t data[5] = {0U, 0U, 0U, 0U, 0U};
    uint8_t i = 0U;
    uint8_t j = 0U;
    uint32_t count = 0U;

    if ((temperature != NULL) && (humidity != NULL))
    {
        gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
        gpio_set_level(DHT11_PIN, 0U);
        esp_rom_delay_us(18000U); 
        gpio_set_level(DHT11_PIN, 1U);
        esp_rom_delay_us(30U);    
        gpio_set_direction(DHT11_PIN, GPIO_MODE_INPUT);

        ret = waitPinState(0U, 80U);
        if (ret == ESP_OK)
        {
            ret = waitPinState(1U, 80U);
            if (ret == ESP_OK)
            {
                ret = waitPinState(0U, 80U);
                if (ret == ESP_OK)
                {
                    while ((i < 5U) && (ret == ESP_OK))
                    {
                        j = 0U;
                        while ((j < 8U) && (ret == ESP_OK))
                        {
                            ret = waitPinState(1U, 70U); 
                            if (ret == ESP_OK)
                            {
                                count = 0U;
                                while ((gpio_get_level(DHT11_PIN) == 1U) && (count < 100U))
                                {
                                    esp_rom_delay_us(1U);
                                    count++;
                                }

                                if (count >= 100U)
                                {
                                    ret = ESP_FAIL;
                                }
                                else
                                {
                                    data[i] = (uint8_t)(data[i] << 1U);
                                    if (count > 40U) 
                                    {
                                        data[i] = (uint8_t)(data[i] | 1U);
                                    }
                                }
                                j++;
                            }
                        }
                        i++;
                    }
                }
            }
        }

        if (ret == ESP_OK)
        {
            if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) == data[4])
            {
                *humidity = data[0];
                *temperature = data[2];
            }
            else
            {
                ret = ESP_FAIL;
            }
        }
    }
    else
    {
        ret = ESP_ERR_INVALID_ARG;
    }

    return ret;
}

void app_main(void)
{
    esp_err_t status = ESP_OK;
    uint8_t temp = 0U;
    uint8_t hum = 0U;
    char tempStr[16] = {0};
    char humStr[16] = {0};

    status = initI2c();
    if (status == ESP_OK)
    {
        status = sendLcdCommand(0xAEU); 
        if (status == ESP_OK) { status = sendLcdCommand(0x20U); } 
        if (status == ESP_OK) { status = sendLcdCommand(0x00U); } 
        
        if (status == ESP_OK) { status = sendLcdCommand(0xC8U); } 
        if (status == ESP_OK) { status = sendLcdCommand(0xA1U); } 

        if (status == ESP_OK) { status = sendLcdCommand(0x8DU); } 
        if (status == ESP_OK) { status = sendLcdCommand(0x14U); } 
        if (status == ESP_OK) { status = sendLcdCommand(0xA4U); } 
        if (status == ESP_OK) { status = sendLcdCommand(0xAFU); } 
    }

    if (status == ESP_OK) 
    { 
        status = clearLcd(); 
    }

    if (status == ESP_OK)
    {
        status = initTimerInterrupt();
    }

    if (status == ESP_OK)
    {
        while (1U == 1U)
        {
            if (s_readSensorFlag != 0U)
            {
                s_readSensorFlag = 0U; 

                status = readDht11(&temp, &hum);
                if (status == ESP_OK)
                {
                    /* Định dạng chuỗi ký tự bằng snprintf chuẩn của C */
                    snprintf(tempStr, sizeof(tempStr), "Temp: %u C", temp);
                    snprintf(humStr, sizeof(humStr), "Hum : %u %%", hum);

                    /* In nhiệt độ ở Hàng 0, Cột 0 */
                    status = setCursor(0U, 0U);
                    if (status == ESP_OK) 
                    { 
                        status = drawString(tempStr); 
                    }

                    /* In độ ẩm ở Hàng 2, Cột 0 (Cách 1 hàng cho dễ nhìn) */
                    if (status == ESP_OK) 
                    { 
                        status = setCursor(0U, 2U); 
                    }
                    if (status == ESP_OK) 
                    { 
                        status = drawString(humStr); 
                    }
                }
            }
            vTaskDelay(10U / portTICK_PERIOD_MS);
        }
    }

    return;
}