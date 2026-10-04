/* =============================================================================
 * Session 06 - ST7796U 3.5" IPS panel over SPI, 480x320 landscape, RGB565.
 *
 * PANEL: LCD Wiki MSP3525 (ST7796U controller), no touch.
 * BUS:   SPI2_HOST, driver/spi_master.h only - deliberately NOT esp_lcd.
 *
 * WIRING (module pin -> ESP32-S3):
 *   VCC -> 5V        GND -> GND
 *   SCK -> GPIO12    SDI(MOSI) -> GPIO11    SDO(MISO) -> GPIO13
 *   LCD_CS -> GPIO10 LCD_RS -> GPIO9        LCD_RST -> GPIO14
 *   LED -> GPIO2     SD_CS -> unconnected
 *
 * Only two functions ever touch LCD_RS: lcd_write_cmd() and lcd_write_data().
 * Everything else goes through them.
 * ========================================================================== */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* =============================================================================
 * 1. Pins and bus
 * ========================================================================== */

#define LCD_HOST        SPI2_HOST
#define PIN_SCK         GPIO_NUM_12
#define PIN_MOSI        GPIO_NUM_11
#define PIN_MISO        GPIO_NUM_13
#define PIN_CS          GPIO_NUM_10
#define PIN_RS          GPIO_NUM_9
#define PIN_RST         GPIO_NUM_14
#define PIN_BK_LIGHT    GPIO_NUM_2

#define RS_LEVEL_CMD    (0)   /* LCD_RS low  = the byte is a command */
#define RS_LEVEL_DATA   (1)   /* LCD_RS high = the byte is data      */
#define RST_LEVEL_ACTIVE   (0)
#define RST_LEVEL_RELEASED (1)
#define BK_LIGHT_ON     (1)

#define LCD_CLK_HZ      (20 * 1000 * 1000)  /* raise once it works */
#define SPI_MODE_CPOL0_CPHA0 (0)
#define SPI_QUEUE_SIZE  (7)

/* =============================================================================
 * 2. Geometry
 * ========================================================================== */

#define LCD_H_RES       (480U)  /* landscape: width  */
#define LCD_V_RES       (320U)  /* landscape: height */

#define BYTES_PER_PIXEL (2U)    /* RGB565 */

/* One SPI transaction carries this many bytes at most.
 *
 * WHY 4096: a full screen is 480*320*2 = 307,200 bytes, far too much for one
 * transaction. 4096 bytes = 2048 pixels = a little over four full 480-pixel
 * rows, so a whole screen takes 75 transactions instead of 153,600. The
 * per-transaction overhead (a few microseconds of setup) is then negligible
 * against the ~1.6 ms it takes to clock 4096 bytes out at 20 MHz. Going larger
 * buys almost nothing and costs DMA-capable RAM, which is the scarce resource
 * on this chip. 4096 is also a comfortable multiple of the 2-byte pixel size,
 * so no pixel is ever split across two transactions.
 */
#define CHUNK_BYTES     (4096U)
#define CHUNK_PIXELS    (CHUNK_BYTES / BYTES_PER_PIXEL)

#define BITS_PER_BYTE   (8U)

/* =============================================================================
 * 3. ST7796U command set (datasheet)
 * ========================================================================== */

#define CMD_SWRESET     (0x01U)
#define CMD_SLPOUT      (0x11U)
#define CMD_INVON       (0x21U)
#define CMD_DISPON      (0x29U)
#define CMD_CASET       (0x2AU)
#define CMD_RASET       (0x2BU)
#define CMD_RAMWR       (0x2CU)
#define CMD_MADCTL      (0x36U)
#define CMD_COLMOD      (0x3AU)

#define COLMOD_RGB565   (0x55U)

/* MADCTL bits */
#define MADCTL_MY       (0x80U)  /* row address order    */
#define MADCTL_MX       (0x40U)  /* column address order */
#define MADCTL_MV       (0x20U)  /* row/column exchange -> landscape */
#define MADCTL_BGR      (0x08U)  /* set = BGR panel order, clear = RGB */

/* Landscape orientation, 480 wide by 320 tall.
 *
 * MV is what rotates the panel. BGR is set because this module's panel wires
 * its subpixels in BGR order - the same value the LCD Wiki vendor demo uses
 * for landscape (0x28).
 *
 * >>> IF RED AND BLUE COME OUT SWAPPED (green correct), clear MADCTL_BGR here.
 * >>> Fixing it in MADCTL rather than by swapping the colour constants is the
 * >>> right place: the constants are correct RGB565 by definition, and the
 * >>> mismatch is a property of the panel's wiring, not of the colours.
 */
#define MADCTL_LANDSCAPE  (MADCTL_MV | MADCTL_BGR)

/* Mandatory settling times, from the ST7796U datasheet. The panel stays blank
   if these are skipped - the controller is still busy internally. */
#define DELAY_AFTER_RST_MS      (10U)
#define DELAY_AFTER_SWRESET_MS  (120U)
#define DELAY_AFTER_SLPOUT_MS   (120U)
#define DELAY_AFTER_DISPON_MS   (100U)

/* =============================================================================
 * 4. Colours (RGB565, high byte first on the wire)
 * ========================================================================== */

#define COLOUR_RED      (0xF800U)
#define COLOUR_GREEN    (0x07E0U)
#define COLOUR_BLUE     (0x001FU)
#define COLOUR_WHITE    (0xFFFFU)
#define COLOUR_BLACK    (0x0000U)

#define BAR_COUNT       (3U)
#define BAR_HOLD_MS     (3000U)
#define COLOUR_HOLD_MS  (1000U)

static const char *TAG = "ST7796";

static spi_device_handle_t lcd_spi;
static uint8_t *chunk_buf;   /* DMA-capable scratch buffer, CHUNK_BYTES long */

/* =============================================================================
 * 5. The only two functions that touch LCD_RS
 * ========================================================================== */

static void lcd_write_cmd(uint8_t cmd)
{
    spi_transaction_t t = {
        .length    = BITS_PER_BYTE,   /* SPI lengths are in BITS, not bytes */
        .tx_buffer = &cmd,
    };

    ESP_ERROR_CHECK(gpio_set_level(PIN_RS, RS_LEVEL_CMD));
    ESP_ERROR_CHECK(spi_device_polling_transmit(lcd_spi, &t));
}

static void lcd_write_data(const uint8_t *buf, size_t len)
{
    if (len == 0U) {
        return;
    }

    spi_transaction_t t = {
        .length    = len * BITS_PER_BYTE,
        .tx_buffer = buf,
    };

    ESP_ERROR_CHECK(gpio_set_level(PIN_RS, RS_LEVEL_DATA));
    ESP_ERROR_CHECK(spi_device_polling_transmit(lcd_spi, &t));
}

/* Convenience for the many single-byte parameters in the init sequence. */
static void lcd_write_data_byte(uint8_t value)
{
    lcd_write_data(&value, 1U);
}

/* =============================================================================
 * 6. Bring-up
 * ========================================================================== */

static void lcd_gpio_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_RS) | (1ULL << PIN_RST) | (1ULL << PIN_BK_LIGHT),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    ESP_ERROR_CHECK(gpio_set_level(PIN_BK_LIGHT, BK_LIGHT_ON));
    ESP_ERROR_CHECK(gpio_set_level(PIN_RS, RS_LEVEL_DATA));
    ESP_ERROR_CHECK(gpio_set_level(PIN_RST, RST_LEVEL_RELEASED));
}

static void lcd_spi_init(void)
{
    spi_bus_config_t buscfg = {
        .sclk_io_num     = PIN_SCK,
        .mosi_io_num     = PIN_MOSI,
        .miso_io_num     = PIN_MISO,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = CHUNK_BYTES,   /* must be >= CHUNK_BYTES or transfers fail at runtime */
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = LCD_CLK_HZ,
        .mode           = SPI_MODE_CPOL0_CPHA0,
        .spics_io_num   = PIN_CS,   /* the driver toggles CS around each transaction */
        .queue_size     = SPI_QUEUE_SIZE,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(LCD_HOST, &devcfg, &lcd_spi));
}

static void lcd_hardware_reset(void)
{
    ESP_ERROR_CHECK(gpio_set_level(PIN_RST, RST_LEVEL_ACTIVE));
    vTaskDelay(pdMS_TO_TICKS(DELAY_AFTER_RST_MS));
    ESP_ERROR_CHECK(gpio_set_level(PIN_RST, RST_LEVEL_RELEASED));
    vTaskDelay(pdMS_TO_TICKS(DELAY_AFTER_RST_MS));
}

static void lcd_init(void)
{
    lcd_hardware_reset();

    lcd_write_cmd(CMD_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(DELAY_AFTER_SWRESET_MS));

    lcd_write_cmd(CMD_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(DELAY_AFTER_SLPOUT_MS));

    lcd_write_cmd(CMD_MADCTL);
    lcd_write_data_byte(MADCTL_LANDSCAPE);

    lcd_write_cmd(CMD_COLMOD);
    lcd_write_data_byte(COLMOD_RGB565);

    /* This IPS panel drives its pixels inverted relative to the controller's
       default. Without INVON, black and white come out swapped. It belongs
       after COLMOD and before DISPON - it is a display-mode setting, so it is
       set while the output is still off. */
    lcd_write_cmd(CMD_INVON);

    lcd_write_cmd(CMD_DISPON);
    vTaskDelay(pdMS_TO_TICKS(DELAY_AFTER_DISPON_MS));

    ESP_LOGI(TAG, "Panel up: %ux%u landscape, MADCTL=0x%02X",
             (unsigned)LCD_H_RES, (unsigned)LCD_V_RES, MADCTL_LANDSCAPE);
}

/* =============================================================================
 * 7. Window + fill
 * ========================================================================== */

/* CASET / RASET take four bytes each: start and end coordinate, high byte
   first. Both ends are inclusive, so a 1-pixel column has x0 == x1. */
static void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t col[4] = {
        (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFFU),
        (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFFU),
    };
    uint8_t row[4] = {
        (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFFU),
        (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFFU),
    };

    lcd_write_cmd(CMD_CASET);
    lcd_write_data(col, sizeof(col));

    lcd_write_cmd(CMD_RASET);
    lcd_write_data(row, sizeof(row));

    lcd_write_cmd(CMD_RAMWR);
}

/* The one and only fill path. A full-screen fill is this called with the full
   screen - there is no second implementation. */
static void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                          uint16_t colour)
{
    if (w == 0U || h == 0U) {
        return;
    }
    if ((uint32_t)x + w > LCD_H_RES || (uint32_t)y + h > LCD_V_RES) {
        ESP_LOGW(TAG, "fill_rect out of bounds, ignored");
        return;
    }

    lcd_set_window(x, y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U));

    /* Pre-fill the scratch buffer once: every chunk of a solid rectangle is
       identical, so this happens outside the transmit loop. High byte first. */
    for (uint32_t i = 0; i < CHUNK_PIXELS; i++) {
        chunk_buf[i * BYTES_PER_PIXEL]        = (uint8_t)(colour >> 8);
        chunk_buf[(i * BYTES_PER_PIXEL) + 1U] = (uint8_t)(colour & 0xFFU);
    }

    uint32_t pixels_left = (uint32_t)w * (uint32_t)h;
    while (pixels_left > 0U) {
        uint32_t n = (pixels_left > CHUNK_PIXELS) ? CHUNK_PIXELS : pixels_left;
        lcd_write_data(chunk_buf, n * BYTES_PER_PIXEL);
        pixels_left -= n;
    }
}

/* =============================================================================
 * 8. app_main
 * ========================================================================== */

static void draw_colour_bars(void)
{
    static const uint16_t bar_colours[BAR_COUNT] = {
        COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE,
    };

    const uint16_t bar_h = (uint16_t)(LCD_V_RES / BAR_COUNT);

    for (uint32_t i = 0; i < BAR_COUNT; i++) {
        /* The last bar absorbs the remainder so no row is left undrawn when
           the height does not divide evenly (320 / 3 = 106 remainder 2). */
        uint16_t h = (i == (BAR_COUNT - 1U))
                     ? (uint16_t)(LCD_V_RES - (bar_h * (BAR_COUNT - 1U)))
                     : bar_h;

        lcd_fill_rect(0U, (uint16_t)(bar_h * i), LCD_H_RES, h, bar_colours[i]);
    }

    ESP_LOGI(TAG, "Colour bars drawn, holding %u ms", (unsigned)BAR_HOLD_MS);
}

void app_main(void)
{
    /* DMA cannot reach ordinary heap on this chip - the buffer handed to the
       SPI driver must come from DMA-capable RAM. */
    chunk_buf = heap_caps_malloc(CHUNK_BYTES, MALLOC_CAP_DMA);
    configASSERT(chunk_buf != NULL);

    lcd_gpio_init();
    lcd_spi_init();
    lcd_init();

    draw_colour_bars();
    vTaskDelay(pdMS_TO_TICKS(BAR_HOLD_MS));

    static const uint16_t cycle[] = {
        COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE, COLOUR_WHITE, COLOUR_BLACK,
    };
    const uint32_t cycle_len = sizeof(cycle) / sizeof(cycle[0]);

    uint32_t i = 0;
    while (1) {
        lcd_fill_rect(0U, 0U, LCD_H_RES, LCD_V_RES, cycle[i]);
        vTaskDelay(pdMS_TO_TICKS(COLOUR_HOLD_MS));

        i++;
        if (i >= cycle_len) {
            i = 0;
        }
    }
}
