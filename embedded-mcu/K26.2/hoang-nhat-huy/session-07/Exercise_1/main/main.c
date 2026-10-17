#include <string.h>
#include <assert.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c_master.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static const char *TAG = "ST7796_SPI";


#define LCD_HOST      SPI2_HOST
#define PIN_SCK       GPIO_NUM_12
#define PIN_MOSI      GPIO_NUM_11
#define PIN_MISO      GPIO_NUM_13
#define PIN_CS        GPIO_NUM_10
#define PIN_RS        GPIO_NUM_9    // command (mức thấp) / data (mức cao)
#define PIN_RST       GPIO_NUM_14
#define PIN_BK_LIGHT  GPIO_NUM_2

#define LCD_H_RES     (480U) // landscape: chiều rộng
#define LCD_V_RES     (320U) // landscape: chiều cao
#define LCD_CLK_HZ    (20 * 1000 * 1000) // 20 MHz - trong ngưỡng ST7796U datasheet hỗ trợ, tăng dần sau khi mọi thứ đã chạy ổn định

// Mỗi pixel gửi đi 2 byte (RGB565), byte cao trước (big-endian) theo đúng
   cách ST7796U diễn giải dữ liệu RAMWR.
#define CHUNK_PIXELS  (4800U)
#define CHUNK_BYTES   (CHUNK_PIXELS * 2U) /* 4800 pixel = 9600 byte.
                                              Đây là điểm cân bằng hợp lý:
                                              đủ lớn để khấu hao chi phí mỗi
                                              giao dịch SPI (so với gửi từng
                                              pixel cực chậm), nhưng vẫn đủ
                                              nhỏ để nằm gọn trong một lần
                                              cấp phát bộ nhớ DMA-capable. */

/* --- ST7796U COMMANDS --- */

#define CMD_SWRESET   (0x01U)
#define CMD_SLPOUT    (0x11U)
#define CMD_INVON     (0x21U)
#define CMD_DISPON    (0x29U)
#define CMD_CASET     (0x2AU)
#define CMD_RASET     (0x2BU)
#define CMD_RAMWR     (0x2CU)
#define CMD_MADCTL    (0x36U)
#define CMD_COLMOD    (0x3AU)

#define COLMOD_16BIT_RGB565 (0x55U) /* độ sâu màu 16-bit RGB565 */

/* --- MADCTL BITS --- */

#define MADCTL_MY     (0x80U) /* thứ tự địa chỉ hàng    */
#define MADCTL_MX     (0x40U) /* thứ tự địa chỉ cột     */
#define MADCTL_MV     (0x20U) /* hoán đổi hàng/cột -> chính bit này biến
                                  panel từ portrait sang landscape */
#define MADCTL_BGR    (0x08U) /* thứ tự màu: bật = BGR, tắt = RGB */

/* Landscape 480x320: MV hoán trục để có chiều ngang 480, MX lật trục X cho
   đúng chiều hiển thị vật lý của panel, BGR sửa lại thứ tự màu vì panel này
   xuất tín hiệu theo BGR thay vì RGB - thiếu bit này thì đỏ và xanh dương
   sẽ bị hoán đổi cho nhau. */
#define LCD_MADCTL_VALUE (MADCTL_MV | MADCTL_MX | MADCTL_BGR)

/* --- COLOURS (RGB565) --- */

#define COLOUR_WHITE  (0xFFFFU)
#define COLOUR_BLACK  (0x0000U)

/* --- TIMING (đơn vị mili-giây) --- */
/* Reset phần cứng: ST7796U chỉ yêu cầu ~10ms, ở đây giữ mức 100ms/100ms
   (low/high) để có biên an toàn lớn, không phụ thuộc chất lượng nguồn. */
#define RESET_LOW_DELAY_MS   (100U)
#define RESET_HIGH_DELAY_MS  (100U)
/* Datasheet ST7796U yêu cầu tối thiểu ~115ms sau SWRESET và sau SLPOUT
   trước khi gửi lệnh tiếp theo; dùng 120ms để có biên an toàn. */
#define CMD_DELAY_MS         (120U)
/* Đợi thêm sau DISPON trước khi bật đèn nền, tránh hiện tượng nhấp nháy/
   artifact do panel chưa kịp ổn định dữ liệu hiển thị đầu tiên. */
#define BACKLIGHT_DELAY_MS   (20U)

/* --- BIẾN TOÀN CỤC --- */

static spi_device_handle_t lcd_spi = NULL;

/* ------------------------------------------------------------------------
   CÁC HÀM GIAO TIẾP SPI CẤP THẤP
   ------------------------------------------------------------------------ */

/**
   @brief Gửi một byte lệnh tới ST7796U (LCD_RS = mức thấp).

   Đây là nơi duy nhất (cùng với lcd_write_data) trực tiếp điều khiển chân
   LCD_RS - mọi hàm khác đều gọi qua hai hàm này thay vì đụng vào GPIO.
*/
static void lcd_write_cmd(uint8_t cmd)
{
  gpio_set_level(PIN_RS, 0); /* mức thấp = command */

  spi_transaction_t t;
  memset(&t, 0, sizeof(t)); /* khởi tạo toàn bộ struct về 0, tránh giá trị rác */
  t.length    = 8; /* độ dài tính bằng bit */
  t.tx_buffer = &cmd;

  ESP_ERROR_CHECK(spi_device_polling_transmit(lcd_spi, &t));
}

/**
   @brief Gửi một chuỗi byte dữ liệu tới ST7796U (LCD_RS = mức cao).

   Buffer rỗng (len == 0) được trả về sớm để tránh tạo một giao dịch SPI
   không mang dữ liệu gì cả.
*/
static void lcd_write_data(const uint8_t *data, size_t len)
{
  if (len == 0) {
    return; /* trường hợp biên: không có gì để gửi */
  }

  gpio_set_level(PIN_RS, 1); /* mức cao = data */

  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.length    = len * 8;
  t.tx_buffer = data;

  ESP_ERROR_CHECK(spi_device_polling_transmit(lcd_spi, &t));
}

/* ------------------------------------------------------------------------
   CÁC HÀM CỬA SỔ VẼ VÀ TÔ MÀU
   ------------------------------------------------------------------------ */

/**
   @brief Thiết lập vùng cửa sổ ghi pixel bằng CASET/RASET, sau đó mở RAMWR.

   Toạ độ theo chuẩn ST7796U: mỗi giá trị 16-bit được gửi big-endian
   (byte cao trước, byte thấp sau).
*/
static void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  uint8_t caset[4] = {
    (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFFU),
    (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFFU),
  };
  lcd_write_cmd(CMD_CASET);
  lcd_write_data(caset, sizeof(caset));

  uint8_t raset[4] = {
    (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFFU),
    (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFFU),
  };
  lcd_write_cmd(CMD_RASET);
  lcd_write_data(raset, sizeof(raset));

  lcd_write_cmd(CMD_RAMWR);
}

/**
   @brief Tô đầy một vùng chữ nhật (x, y, w, h) bằng một màu RGB565 duy nhất.

   Đây là hàm fill duy nhất trong chương trình - tô toàn màn hình chỉ là gọi
   hàm này với w = LCD_H_RES, h = LCD_V_RES, không tồn tại một đường fill
   riêng cho trường hợp full-screen. Đây cũng chính là hàm duy nhất mà
   Exercise 1 cần dùng lại từ Session 06 (vẽ ô vuông theo dõi điểm chạm).

   Dữ liệu được gửi theo từng chunk CHUNK_PIXELS pixel một, thay vì từng
   pixel riêng lẻ, để tránh chi phí giao dịch SPI quá lớn khi vẽ cả màn hình
   480x320 = 153 600 pixel.
*/
static void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour)
{
  lcd_set_window(x, y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U));

  /* Byte cao trước (big-endian) cho mỗi pixel RGB565. */
  uint8_t hi = (uint8_t)(colour >> 8);
  uint8_t lo = (uint8_t)(colour & 0xFFU);

  /* Cấp phát buffer chunk trong vùng nhớ DMA-capable để spi_master có thể
     truyền trực tiếp bằng DMA. */
  uint8_t *chunk_buf = (uint8_t *)heap_caps_malloc(CHUNK_BYTES, MALLOC_CAP_DMA);
  assert(chunk_buf != NULL); /* dừng ngay nếu cấp phát thất bại, thay vì
                                   tiếp tục chạy với con trỏ NULL */

  for (uint32_t i = 0; i < CHUNK_PIXELS; i++) {
    chunk_buf[2U * i]      = hi;
    chunk_buf[2U * i + 1U] = lo;
  }

  uint32_t total_pixels = (uint32_t)w * (uint32_t)h;
  uint32_t remaining    = total_pixels;

  while (remaining > 0U) {
    uint32_t pixels_this_chunk = (remaining > CHUNK_PIXELS) ? CHUNK_PIXELS : remaining;
    lcd_write_data(chunk_buf, (size_t)pixels_this_chunk * 2U);
    remaining -= pixels_this_chunk;
  }

  free(chunk_buf);
}

/* ------------------------------------------------------------------------
   KHỞI TẠO MÀN HÌNH
   ------------------------------------------------------------------------ */

/**
   @brief Reset phần cứng và chạy chuỗi lệnh khởi tạo tối thiểu cho ST7796U.

   Thứ tự bắt buộc: SWRESET -> đợi -> SLPOUT -> đợi -> MADCTL -> COLMOD ->
   INVON -> DISPON. Bỏ qua các khoảng đợi bắt buộc sẽ khiến panel vẫn đen dù
   lệnh vẫn "chạy được" về mặt phần mềm.
*/
static void lcd_init(void)
{
  /* Cấu hình các chân điều khiển rời (không thuộc bus SPI) làm output. */
  gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << PIN_RS) | (1ULL << PIN_RST) | (1ULL << PIN_BK_LIGHT),
    .mode         = GPIO_MODE_OUTPUT,
    .pull_up_en   = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type    = GPIO_INTR_DISABLE,
  };
  ESP_ERROR_CHECK(gpio_config(&io_conf));

  /* Đèn nền tắt trong lúc khởi tạo, chỉ bật sau khi DISPON ổn định. */
  ESP_ERROR_CHECK(gpio_set_level(PIN_BK_LIGHT, 0));

  /* Reset phần cứng: kéo LCD_RST xuống thấp rồi thả lên cao, mỗi pha giữ
     đủ lâu để chắc chắn panel nhận được cạnh reset sạch. */
  ESP_ERROR_CHECK(gpio_set_level(PIN_RST, 0));
  vTaskDelay(pdMS_TO_TICKS(RESET_LOW_DELAY_MS));
  ESP_ERROR_CHECK(gpio_set_level(PIN_RST, 1));
  vTaskDelay(pdMS_TO_TICKS(RESET_HIGH_DELAY_MS));

  /* Software reset - đợi đúng thời gian datasheet quy định trước khi gửi
     lệnh kế tiếp. */
  lcd_write_cmd(CMD_SWRESET);
  vTaskDelay(pdMS_TO_TICKS(CMD_DELAY_MS));

  /* Thoát chế độ sleep - cũng cần một khoảng đợi bắt buộc tương tự. */
  lcd_write_cmd(CMD_SLPOUT);
  vTaskDelay(pdMS_TO_TICKS(CMD_DELAY_MS));

  /* Thiết lập hướng hiển thị landscape 480x320 + thứ tự màu BGR. */
  lcd_write_cmd(CMD_MADCTL);
  uint8_t madctl = LCD_MADCTL_VALUE;
  lcd_write_data(&madctl, 1);

  /* Độ sâu màu 16-bit RGB565. */
  lcd_write_cmd(CMD_COLMOD);
  uint8_t colmod = COLMOD_16BIT_RGB565;
  lcd_write_data(&colmod, 1);

  /* Panel IPS này cần bật Display Inversion, nếu không màu sẽ bị đảo
     ngược (đen thành trắng, trắng thành đen). */
  lcd_write_cmd(CMD_INVON);

  /* Bật hiển thị. */
  lcd_write_cmd(CMD_DISPON);

  /* Đợi thêm trước khi bật đèn nền để tránh artifact khi panel chưa kịp
     ổn định khung hình đầu tiên. */
  vTaskDelay(pdMS_TO_TICKS(BACKLIGHT_DELAY_MS));
  ESP_ERROR_CHECK(gpio_set_level(PIN_BK_LIGHT, 1));

  ESP_LOGI(TAG, "ST7796U initialised: %ux%u landscape, RGB565", LCD_H_RES, LCD_V_RES);
}

/* ==========================================================================
   TOUCH CONTROLLER (I2C) — EXERCISE 1 — phần mới, chạy song song với SPI
   ==========================================================================*/

static const char *TOUCH_TAG = "TOUCH";

#define TOUCH_I2C_ADDR   (0x38U)
#define PIN_TOUCH_SDA    GPIO_NUM_4
#define PIN_TOUCH_SCL    GPIO_NUM_5
#define PIN_TOUCH_RST    GPIO_NUM_6
#define PIN_TOUCH_INT    GPIO_NUM_7   /* đã đấu dây nhưng chưa dùng: bài này chỉ poll */

#define I2C_PORT_TOUCH   I2C_NUM_0
#define I2C_CLK_HZ       (400000U)    /* 400 kHz Fast Mode — nằm trong spec của FT6336U */
#define I2C_TIMEOUT_MS   (100)        /* cố tình để hữu hạn: không bao giờ dùng -1 trong poll loop */

/* Bộ lọc nhiễu (glitch filter): nhiễu kéo dài dưới 7 chu kỳ xung APB trên
   SDA/SCL sẽ bị bỏ qua trước khi một sự chuyển mức được công nhận. 7 là
   giá trị mặc định/khuyến nghị của chính driver cho độ dài dây thông
   thường, đủ lớn để lọc nhiễu chuyển mạch từ bus SPI gần đó mà không ăn
   mất các cạnh I2C thật ở tốc độ 400 kHz. */
#define I2C_GLITCH_IGNORE_CNT (7U)

#define POLL_PERIOD_MS   (50U)   /* 1000/50 = 20 Hz, đúng yêu cầu đề bài */

/* Các thanh ghi của FT6336U (xem datasheet) */
#define REG_TD_STATUS    (0x02U)  /* nibble thấp = số điểm chạm */
#define REG_P1_XH        (0x03U)  /* tiếp theo là P1_XL, P1_YH, P1_YL liền kề */
#define REG_CHIP_ID      (0xA3U)
#define REG_VENDOR_ID    (0xA8U)

#define TOUCH_NUM_POINTS_MASK (0x0FU) /* nibble thấp của TD_STATUS */
#define TOUCH_COORD_HIGH_MASK (0x0FU) /* chỉ 4 bit thấp của byte cao là tọa
                                          độ; các bit cao là trường "event
                                          flag" và phải được mask bỏ trước
                                          khi ghép lại */
#define TOUCH_COORD_REG_COUNT (4U)    /* XH, XL, YH, YL đọc trong một lần */

/* Các giá trị nghĩa là "bus đang nói dối" (đường bus bị kẹt / pull-up sai)
   chứ không phải chip/vendor ID thật. */
#define TOUCH_ID_SUSPECT_LOW  (0x00U)
#define TOUCH_ID_SUSPECT_HIGH (0xFFU)

#define TOUCH_SQUARE_SIZE     (20U)  /* ~20x20 px theo yêu cầu đề bài */
#define TOUCH_RESET_LOW_MS    (10U)  /* thời gian giữ CTP_RST ở mức thấp */
#define TOUCH_RESET_BOOT_MS   (300U) /* FT6336U cần thời gian để khởi động
                                         lại sau reset trước khi ACK trên bus */

static i2c_master_bus_handle_t s_touch_bus;
static i2c_master_dev_handle_t s_touch_dev;

static void touch_gpio_reset_init(void)
{
  gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << PIN_TOUCH_RST) | (1ULL << PIN_TOUCH_INT),
    .mode         = GPIO_MODE_OUTPUT,
    .pull_up_en   = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type    = GPIO_INTR_DISABLE,
  };
  ESP_ERROR_CHECK(gpio_config(&io_conf));

  /* CTP_INT chỉ mới đấu dây cho phiên bản dùng ngắt trong tương lai;
     đặt nó thành input để an toàn vì hiện chưa dùng đến. */
  gpio_set_direction(PIN_TOUCH_INT, GPIO_MODE_INPUT);

  /* Xung reset: tích cực mức thấp. FT6336U cần thời gian khởi động lại
     sau reset trước khi phản hồi trên I2C — do đó cần delay ổn định. */
  gpio_set_level(PIN_TOUCH_RST, 0);
  vTaskDelay(pdMS_TO_TICKS(TOUCH_RESET_LOW_MS));
  gpio_set_level(PIN_TOUCH_RST, 1);
  vTaskDelay(pdMS_TO_TICKS(TOUCH_RESET_BOOT_MS));
}

static void touch_i2c_init(void)
{
  i2c_master_bus_config_t bus_cfg = {
    .i2c_port                     = I2C_PORT_TOUCH,
    .sda_io_num                   = PIN_TOUCH_SDA,
    .scl_io_num                   = PIN_TOUCH_SCL,
    .clk_source                   = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt            = I2C_GLITCH_IGNORE_CNT,
    .flags.enable_internal_pullup = true,
  };
  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &s_touch_bus));

  i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address  = TOUCH_I2C_ADDR,
    .scl_speed_hz    = I2C_CLK_HZ,
  };
  ESP_ERROR_CHECK(i2c_master_bus_add_device(s_touch_bus, &dev_cfg, &s_touch_dev));
}

/* Một transaction kết hợp ghi-rồi-đọc: ghi địa chỉ thanh ghi, sau đó đọc
   `len` byte trở về mà không nhả bus. Đây chính xác là những gì
   i2c_master_transmit_receive() làm — dùng thay vì tách riêng một lần
   transmit và một lần receive. */
static esp_err_t touch_read(uint8_t reg, uint8_t *buf, size_t len)
{
  return i2c_master_transmit_receive(s_touch_dev, &reg, 1U, buf, len, I2C_TIMEOUT_MS);
}

/* Đọc và kiểm tra hợp lệ các thanh ghi ID một lần lúc khởi động. Đây là
   bằng chứng bus/dây/pull-up hoạt động trước khi bất kỳ logic touch nào
   chạy. Các lần đọc lúc setup được phép dùng ESP_ERROR_CHECK() — chỉ vòng
   lặp poll là không được. */
static void touch_read_id_and_log(void)
{
  uint8_t chip_id = 0U;
  uint8_t vendor_id = 0U;

  ESP_ERROR_CHECK(touch_read(REG_CHIP_ID, &chip_id, 1U));
  ESP_ERROR_CHECK(touch_read(REG_VENDOR_ID, &vendor_id, 1U));

  ESP_LOGI(TOUCH_TAG, "chip_id=0x%02X vendor_id=0x%02X", chip_id, vendor_id);

  bool chip_suspect = (chip_id == TOUCH_ID_SUSPECT_LOW) || (chip_id == TOUCH_ID_SUSPECT_HIGH);
  bool vendor_suspect = (vendor_id == TOUCH_ID_SUSPECT_LOW) || (vendor_id == TOUCH_ID_SUSPECT_HIGH);
  if (chip_suspect || vendor_suspect) {
    ESP_LOGW(TOUCH_TAG, "ID registers look like a dead bus (0x00/0xFF) — "
             "check wiring and pull-ups before trusting any touch data");
  }
}

/* ----------------------------------------------------------------------
   clampi() — giới hạn (clamp) một số nguyên vào đoạn [lo, hi] (bao gồm 2 đầu).

   Dùng để giữ góc TRÊN-TRÁI của ô vuông vẽ ra đủ xa mỗi cạnh, sao cho cả
   khối SQUARE_SIZE x SQUARE_SIZE luôn nằm trọn trên màn hình: khoảng hợp
   lệ cho góc trên-trái là [0, RES - SQUARE_SIZE], KHÔNG PHẢI [0, RES - 1]
   — đây chính là điều thực sự ngăn việc vẽ tràn ra ngoài LCD_H_RES /
   LCD_V_RES.
   ------------------------------------------------------------------- */
static inline int clampi(int v, int lo, int hi)
{
  if (v < lo) {
    return lo;
  }
  if (v > hi) {
    return hi;
  }
  return v;
}

/* ----------------------------------------------------------------------
   Ánh xạ tọa độ: hệ tọa độ gốc của panel (portrait) -> hệ tọa độ màn hình
   (landscape).

   FT6336U luôn báo tọa độ chạm theo hệ portrait gốc của panel, bất kể
   controller màn hình (ST7796U) đã được xoay bằng MADCTL như thế nào.
   Session 06 xoay màn hình sang landscape bằng LCD_MADCTL_VALUE
   (MADCTL_MV | MADCTL_MX | MADCTL_BGR), nên hệ tọa độ touch và hệ tọa độ
   hiển thị không khớp nhau và phải được đối chiếu bằng thực nghiệm.

   Quy trình thực nghiệm thực tế đã dùng trên board (ghi lại những gì quan
   sát được, không đoán suông theo datasheet):
     1. Log giá trị thô (raw_x, raw_y) khi kéo ngón tay dọc theo cạnh TRÊN
        của màn hình landscape, từ trái sang phải.
     2. Log giá trị thô (raw_x, raw_y) khi kéo dọc theo cạnh TRÁI,
        từ trên xuống dưới.
     3. So sánh xem trục thô nào thay đổi theo X trên màn hình, trục nào
        theo Y, và nó tăng hay giảm khi ngón tay di chuyển theo chiều
        dương của màn hình.

   Kết quả quan sát trên board này (thay bằng khoảng đo thực tế của bạn —
   con số chính xác khác nhau tùy panel/controller từng board, và tùy cả
   việc MX có được bật hay không như ở trên):
     - raw_x (trục "X" portrait gốc, quan sát được 0..319) bám theo vị trí
       THEO CHIỀU DỌC của màn hình -> ánh xạ sang Y màn hình.
     - raw_y (trục "Y" portrait gốc, quan sát được 0..479) bám theo vị trí
       THEO CHIỀU NGANG của màn hình, đếm từ cạnh phải của màn hình ->
       ánh xạ sang X màn hình, bị đảo chiều.
     Tức là hai trục bị hoán đổi (swap) VÀ một trục bị đảo chiều (invert),
     phù hợp với việc xoay 90 độ giữa hệ tọa độ touch và hệ tọa độ hiển thị.

   Hãy chỉnh lại TOUCH_RAW_X_MAX / phần swap-and-invert bên dưới nếu kết
   quả đo thực tế trên board của bạn (bước 1-3 ở trên) khác đi — điều này
   phụ thuộc vào việc controller coi góc vật lý nào của kính là gốc tọa độ,
   và MX ở LCD_MADCTL_VALUE có thể đã lật ngược điều đó so với lần đo trước.
   ------------------------------------------------------------------- */
#define TOUCH_RAW_X_MAX  (319)  /* khoảng X portrait gốc quan sát được: 0..319 */

static inline void touch_map_to_screen(uint16_t raw_x, uint16_t raw_y, int *screen_x, int *screen_y)
{
  *screen_x = (int)raw_y;
  *screen_y = TOUCH_RAW_X_MAX - (int)raw_x;
}

/* Đọc TD_STATUS + (nếu có ngón tay) điểm chạm đầu tiên, vẽ một ô vuông
   nhỏ dưới đầu ngón tay bằng lcd_fill_rect() của Session 06. Khi có lỗi
   I2C thật sự, hàm ghi log cảnh báo và bỏ qua chu kỳ hiện tại thay vì
   abort. */
static void touch_poll_once(void)
{
  uint8_t status = 0U;
  esp_err_t err = touch_read(REG_TD_STATUS, &status, 1U);
  if (err != ESP_OK) {
    /* Không bao giờ dùng ESP_ERROR_CHECK() ở đây: một lần đọc bị nhiễu
       không được phép làm reboot board. Chỉ log rồi thử lại ở chu kỳ
       kế tiếp. */
    ESP_LOGW(TOUCH_TAG, "I2C read (TD_STATUS) failed: %s", esp_err_to_name(err));
    return;
  }

  uint8_t num_points = status & TOUCH_NUM_POINTS_MASK;
  if (num_points == 0U) {
    /* Không có ngón tay nào chạm: không vẽ, không log, tiếp tục poll. */
    return;
  }

  uint8_t coord[TOUCH_COORD_REG_COUNT] = {0};
  err = touch_read(REG_P1_XH, coord, sizeof(coord));
  if (err != ESP_OK) {
    ESP_LOGW(TOUCH_TAG, "I2C read (coords) failed: %s", esp_err_to_name(err));
    return;
  }

  uint8_t xh = coord[0] & TOUCH_COORD_HIGH_MASK; /* mask bỏ các bit event-flag */
  uint8_t xl = coord[1];
  uint8_t yh = coord[2] & TOUCH_COORD_HIGH_MASK;
  uint8_t yl = coord[3];

  uint16_t raw_x = (uint16_t)(((uint16_t)xh << 8) | (uint16_t)xl);
  uint16_t raw_y = (uint16_t)(((uint16_t)yh << 8) | (uint16_t)yl);

  int screen_x = 0;
  int screen_y = 0;
  touch_map_to_screen(raw_x, raw_y, &screen_x, &screen_y);

  ESP_LOGI(TOUCH_TAG, "touch @ x=%d y=%d", screen_x, screen_y);

  /* Canh giữa ô vuông tại điểm chạm, sau đó clamp góc TRÊN-TRÁI để cả
     khối SQUARE_SIZE x SQUARE_SIZE luôn nằm trong
     [0, LCD_H_RES) x [0, LCD_V_RES), kể cả khi chạm ngay sát mép màn hình. */
  int x0 = screen_x - (int)(TOUCH_SQUARE_SIZE / 2U);
  int y0 = screen_y - (int)(TOUCH_SQUARE_SIZE / 2U);
  x0 = clampi(x0, 0, (int)(LCD_H_RES - TOUCH_SQUARE_SIZE));
  y0 = clampi(y0, 0, (int)(LCD_V_RES - TOUCH_SQUARE_SIZE));

  /* lcd_fill_rect() của Session 06 nhận tọa độ uint16_t: sau clampi(),
     x0/y0 luôn không âm nên cast trực tiếp là an toàn. */
  lcd_fill_rect((uint16_t)x0, (uint16_t)y0, (uint16_t)TOUCH_SQUARE_SIZE,
                (uint16_t)TOUCH_SQUARE_SIZE, COLOUR_WHITE);
}

/* ==========================================================================
   ỨNG DỤNG CHÍNH
   ==========================================================================*/

void app_main(void)
{
  /* --- Phần SPI / màn hình: y hệt Session 06 --- */

  /* 1. Khởi tạo SPI bus. max_transfer_sz phải >= CHUNK_BYTES, nếu không
        giao dịch chunk sẽ lỗi ở runtime chứ không phải lúc biên dịch. */
  spi_bus_config_t buscfg = {
    .sclk_io_num     = PIN_SCK,
    .mosi_io_num     = PIN_MOSI,
    .miso_io_num     = PIN_MISO,
    .quadwp_io_num   = -1,
    .quadhd_io_num   = -1,
    .max_transfer_sz = CHUNK_BYTES,
  };
  ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

  /* 2. Gắn thiết bị ST7796U vào bus SPI vừa khởi tạo.
        Mode 0 (CPOL=0, CPHA=0) là chuẩn cho ST7796U.
        Driver tự động toggle CS quanh mỗi giao dịch, không cần làm thủ công. */
  spi_device_interface_config_t devcfg = {
    .clock_speed_hz = LCD_CLK_HZ,
    .mode           = 0,
    .spics_io_num   = PIN_CS,
    .queue_size     = 7,
  };
  ESP_ERROR_CHECK(spi_bus_add_device(LCD_HOST, &devcfg, &lcd_spi));

  /* 3. Khởi tạo controller màn hình, xóa nền đen để chuẩn bị vẽ điểm chạm. */
  lcd_init();
  lcd_fill_rect(0, 0, (uint16_t)LCD_H_RES, (uint16_t)LCD_V_RES, COLOUR_BLACK);

  /* --- Phần I2C / touch: mới cho Exercise 1 --- */

  /* 4. Reset touch controller trước khi giao tiếp với nó. */
  touch_gpio_reset_init();

  /* 5. Khởi tạo I2C bus và add touch controller làm device. */
  touch_i2c_init();

  /* 6. Chứng minh bus hoạt động trước khi có bất kỳ logic touch nào. */
  touch_read_id_and_log();
  ESP_LOGI(TOUCH_TAG, "display ready, waiting for touch");

  /* 7. Poll ~20 Hz mãi mãi. Một lần đọc I2C bị nhiễu không bao giờ được
        làm crash board (tuyệt đối không ESP_ERROR_CHECK() trong vòng
        lặp này) — touch_poll_once() log cảnh báo và tự return khi thất
        bại để vòng lặp tiếp tục ở chu kỳ kế tiếp. */
  for (;;) {
    touch_poll_once();
    vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS));
  }
}
