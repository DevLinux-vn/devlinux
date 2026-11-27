#include <stdio.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#define COLOUR_WHITE    (0xFFFFU)
#define COLOUR_BLACK    (0x0000U)
#define BARS_HOLD_MS    (3000U)
#define COLOUR_HOLD_MS  (1000U)
#define LCD_HOST      SPI2_HOST
#define PIN_SCK       GPIO_NUM_12
#define PIN_MOSI      GPIO_NUM_11
#define PIN_MISO      GPIO_NUM_13
#define PIN_CS        GPIO_NUM_10
#define PIN_RS        GPIO_NUM_9
#define PIN_RST       GPIO_NUM_14
#define PIN_BK_LIGHT  GPIO_NUM_2
#define LCD_H_RES     (480U) /* Hướng ngang: chiều rộng */
#define LCD_V_RES     (320U) /* Hướng ngang: chiều cao */
#define LCD_CLK_HZ    (20 * 1000 * 1000)
/* Bắt đầu ở tần số này; tăng lên sau khi màn hình hoạt động */

#define CMD_SWRESET   (0x01U)
#define CMD_SLPOUT    (0x11U)
#define CMD_INVON     (0x21U)
#define CMD_DISPON    (0x29U)
#define CMD_CASET     (0x2AU)
#define CMD_RASET     (0x2BU)
#define CMD_RAMWR     (0x2CU)
#define CMD_MADCTL    (0x36U)
#define CMD_COLMOD    (0x3AU)
#define CHUNK_BYTES (LCD_H_RES * sizeof(uint16_t))
/* Các bit MADCTL — kết hợp chúng để có hướng hiển thị mong muốn */
#define MADCTL_MY     (0x80U) /* Thứ tự địa chỉ hàng */
#define MADCTL_MX     (0x40U) /* Thứ tự địa chỉ cột */
#define MADCTL_MV     (0x20U) /* Hoán đổi hàng/cột -> tạo hướng ngang */
#define MADCTL_BGR    (0x08U) /* Thứ tự màu: đặt bit = BGR, xóa bit = RGB */

#define COLOUR_RED    (0xF800U)
#define COLOUR_GREEN  (0x07E0U)
#define COLOUR_BLUE   (0x001FU)
static const char* TAG = "ST7796_BASIC";

void st7796_send_cmd(uint8_t cmd){
    if(PIN_RS >= 0 )
    gpio_set_level(PIN_RS, 0);
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    ESP_ERROR_CHECK(spi_device_polling_transmit(s_spi, &t));
}

void st7796_send_data(const uint8_t* data, size_t len){
    if (len ==0) return;
    if (PIN_RS >= 0 ){
        gpio_set_level(PIN_RS, 1);
    }
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len*8;
    t.tx_buffer = data;
    ESP_ERROR_CHECK(spi_device_polling_transmit(s_spi, &t));
}


static spi_device_handle_t s_spi = NULL;
//khoi tao st7796
esp_err_t st7796_init(){
    ESP_LOGI(TAG, "Initializing ST7796 (MOSI = %d, SCLK = %d, CS = %d, DC = %d, RST = %d, BLK = %d)",
                PIN_MOSI,
                PIN_SCK,
                PIN_CS,
                PIN_RS,
                PIN_RST,
                PIN_BK_LIGHT);
    gpio_config_t ioconf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    uint64_t pin_mask = 0;
    if(PIN_RS>=0)
        pin_mask |= 1ULL << (PIN_RS);
    if(PIN_RST >= 0)
        pin_mask |= 1ULL << (PIN_RST);
    if(PIN_BK_LIGHT >= 0)
        pin_mask |= 1ULL << (PIN_BK_LIGHT);


    if(pin_mask > 0){
        io_conf.pin_bit_mask = pin_mask;
        gpio_config(&io_conf);
    }
    //config bus
    spi_bus_config_t buscfg = {
    .sclk_io_num     = PIN_SCK,
    .mosi_io_num     = PIN_MOSI,
    .miso_io_num     = PIN_MISO,
    .quadwp_io_num   = -1,
    .quadhd_io_num   = -1,
    .max_transfer_sz = CHUNK_BYTES, /* Phải >= kích thước khối truyền */
};

    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
        if (ret != ESP_OK) {
             ESP_LOGE(TAG, "spi_bus_initialize failed: %s", esp_err_to_name(ret));
            return ret;
        }
    
    ESP_ERROR_CHECK(gpio_set_level(PIN_BK_LIGHT, 1));
   
//config device
    spi_device_interface_config_t devcfg = {
    .clock_speed_hz = LCD_CLK_HZ,
    .mode           = 0,
    .spics_io_num   = PIN_CS, /* Driver tự điều khiển CS quanh mỗi giao dịch */
    .queue_size     = 7,
};
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &s_spi);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "spi_bus_add_device failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    //Hardware reset
    if(PIN_RST >= 0){
        gpio_set_level(PIN_RST, 0);
        vTaskDelay(pdMS_TO_TICKS(50));
        gpio_set_level(PIN_RST, 1);
        vTaskDelay(pdMS_TO_TICKS(120));
    }

    //ST7796 initialize
    st7796_send_cmd(CMD_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));
    st7796_send_cmd(CMD_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(120));
    st7796_send_cmd(CMD_COLMOD);
    uint8_t colmod = 0x55;
    st7796_send_data(&colmod, 1);
    
    st7796_send_cmd(CMD_MADCTL);
    uint8_t madctl = 0x20 | 0x80 | 0x40 | 0x08;
    st7796_send_data(&madctl, 1);

   st7796_send_cmd(CMD_INVON);
st7796_send_cmd(CMD_DISPON);
vTaskDelay(pdMS_TO_TICKS(50));

    st7796_send_cmd(0x13); // Normal display ON
    vTaskDelay(pdMS_TO_TICKS(10));

    st7796_send_cmd(0x29); // Display ON
    vTaskDelay(pdMS_TO_TICKS(50));
    ESP_LOGI(TAG,
             "ST7796 initialized successfully (%dx%d @ %d MHz).",
             LCD_H_RES,
             LCD_V_RES,
             LCD_CLK_HZ / (1000 * 1000));
    return ESP_OK;
}

void st7796_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){

    //CASET
    st7796_send_cmd(CMD_CASET);
    uint8_t data_x[4] = {(uint8_t) (x0 >>8), (uint8_t) (x0 & 0xFF),(uint8_t) (x1 >>8), (uint8_t) (x1 & 0xFF) };
    st7796_send_data(data_x, sizeof(data_x));

    //RASET
    st7796_send_cmd(CMD_RASET);
    uint8_t data_y[4] = {(uint8_t) (y0 >>8), (uint8_t) (y0 & 0xFF),(uint8_t) (y1 >>8), (uint8_t) (y1 & 0xFF) };
    st7796_send_data(data_y, sizeof(data_y));

    //Memory write
    st7796_send_cmd(CMD_RAMWR);
}

void st7796_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){

    if(x >= LCD_H_RES || y >= LCD_V_RES || w == 0 || h == 0) return;

    if (x + w - 1 >= LCD_H_RES) w = LCD_H_RES - x;
    if (y + h - 1 >= LCD_V_RES) h = LCD_V_RES - y;

    st7796_set_window(x, y, x + w - 1, y + h - 1);
    
    uint16_t color_be = (color >> 8) | (color << 8);

    static uint16_t line_buffer[LCD_H_RES];

    for(int i = 0; i < w; i++){

        line_buffer[i] = color_be;
    }

    for (int row = 0; row < h; row++){
        st7796_send_data((uint8_t*)line_buffer, w*sizeof(uint16_t) );

    }
}

void st7796_draw_pixel()



void app_main(void)
{
    ESP_ERROR_CHECK(st7796_init());

    /*
     * Chia chiều cao thành ba dải.
     * 320 không chia hết cho 3 nên chiều cao là 106, 107, 107.
     */
    const uint16_t y1 = (uint16_t)(LCD_V_RES / 3U);
    const uint16_t y2 = (uint16_t)((2U * LCD_V_RES) / 3U);

    st7796_fill_rect(
        0, 0, LCD_H_RES, y1, COLOUR_RED
    );

    st7796_fill_rect(
        0, y1, LCD_H_RES,
        (uint16_t)(y2 - y1), COLOUR_GREEN
    );

    st7796_fill_rect(
        0, y2, LCD_H_RES,
        (uint16_t)(LCD_V_RES - y2), COLOUR_BLUE
    );

    /* Giữ ba dải màu trong 3 giây sau khi vẽ xong. */
    vTaskDelay(pdMS_TO_TICKS(BARS_HOLD_MS));

    static const uint16_t colours[] = {
        COLOUR_RED,
        COLOUR_GREEN,
        COLOUR_BLUE,
        COLOUR_WHITE,
        COLOUR_BLACK,
    };

    const size_t colour_count =
        sizeof(colours) / sizeof(colours[0]);

    while (1) {
        for (size_t i = 0; i < colour_count; ++i) {
            /* Tô toàn màn hình bằng chính hàm fill_rect(). */
            st7796_fill_rect(
                0, 0, LCD_H_RES, LCD_V_RES, colours[i]
            );

            vTaskDelay(pdMS_TO_TICKS(COLOUR_HOLD_MS));
        }
    }
}