
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "btn7seg";


#define DISPLAY_COMMON_ANODE   (0)

#define SEG_A_PIN   (4U)
#define SEG_B_PIN   (5U)
#define SEG_C_PIN   (6U)
#define SEG_D_PIN   (7U)
#define SEG_E_PIN   (15U)
#define SEG_F_PIN   (16U)
#define SEG_G_PIN   (17U)
#define BTN_PIN     (14U)

#define SO_DOAN (7U)
static const uint32_t CHAN_DOAN[SO_DOAN] = {
  SEG_A_PIN, SEG_B_PIN, SEG_C_PIN, SEG_D_PIN, SEG_E_PIN, SEG_F_PIN, SEG_G_PIN
};

static const uint8_t BANG_SO[10] = {
  0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U,
  0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU,
};

//Cac moc thoi gian
#define CHU_KY_QUET_MS   (5U)    //bao lau doc chan 1 lan
#define DEBOUNCE_MS      (25U)   //muc tin hieu on dinh trong bao nhieu ms moi tinh la that
#define DOUBLE_CLICK_MS  (350U)  //khoang cach thoi gian giua 2 lan click de tinh la nhan doi
#define LONG_PRESS_MS    (800U)  //giu nut trong bao lau thi bat dau dem
#define REPEAT_MS        (500U)  //moi lan tu dong tang cach nhau bao lau

//Dia chi thanh ghi
#define GPIO_BASE     (0x60004000UL)
#define IO_MUX_BASE   (0x60009000UL)

#define GPIO_OUT_REG          (GPIO_BASE + 0x0004UL)
#define GPIO_OUT_W1TS_REG     (GPIO_BASE + 0x0008UL)  //ghi 1 vao bit nao thi bat bit do len 1
#define GPIO_OUT_W1TC_REG     (GPIO_BASE + 0x000CUL)  //ghi 1 vao bit nao thi ha bit do ve 0
#define GPIO_ENABLE_W1TS_REG  (GPIO_BASE + 0x0024UL)  //bat che do output cho chan
#define GPIO_ENABLE_W1TC_REG  (GPIO_BASE + 0x0028UL)  //tat che do output cho chan
#define GPIO_IN_REG           (GPIO_BASE + 0x003CUL)  //doc muc dien ap cho chan

#define GPIO_FUNC_OUT_SEL(pin)  (*(volatile uint32_t *)(GPIO_BASE + 0x0554UL + 4UL * (pin)))
#define MATRIX_GPIO_TRUC_TIEP   (256UL)        //bo qua matrix, dieu khien thang tu GPIO_OUT_REG
#define MATRIX_OE_TU_PHAN_MEM   (1UL << 10U)   //lay bit output-enable tu GPIO_ENABLE thay vi ngoai vi

//Dia chi thanh ghi IO_MUX cua tung chan
#define IOMUX(offset) (IO_MUX_BASE + (offset))
static const uint32_t IO_MUX_CUA_CHAN[19] = {
  [4]  = IOMUX(0x14UL),
  [5]  = IOMUX(0x18UL),
  [6]  = IOMUX(0x1CUL),
  [7]  = IOMUX(0x20UL),
  [14] = IOMUX(0x3CUL),
  [15] = IOMUX(0x40UL),
  [16] = IOMUX(0x44UL),
  [17] = IOMUX(0x48UL),
};

// Vi tri bit trong thanh ghi IO_MUX
#define IOMUX_CHON_HAM_S   (12U)                 // vi tri bat dau cua truong chon chuc nang
#define IOMUX_CHON_HAM_M   (0x7U << IOMUX_CHON_HAM_S)
#define IOMUX_HAM_GPIO     (1U)                   // gia tri = 1 nghia la "dung nhu GPIO thuong"
#define IOMUX_KEO_XUONG_B  (1U << 7U)             // bat pull-down
#define IOMUX_KEO_LEN_B    (1U << 8U)             // bat pull-up
#define IOMUX_CHO_DOC_B    (1U << 9U)             // bat bo dem ngo vao (de doc duoc muc tin hieu)

//Macro doc/ghi 1 thanh ghi 32-bit qua con tro volatile
#define REG32(dia_chi) (*(volatile uint32_t *)(dia_chi))

//Cau hinh chan

static void cau_hinh_chan_ra(uint32_t chan) //cau hinh 1 chan thanh ngo ra GPIO thuong, muc thap ban dau
{
  uint32_t dia_chi_iomux = IO_MUX_CUA_CHAN[chan];

  //IO_MUX - chon chuc nang GPIO cho chan nay
  uint32_t gia_tri = REG32(dia_chi_iomux);
  gia_tri &= ~IOMUX_CHON_HAM_M;
  gia_tri |= (IOMUX_HAM_GPIO << IOMUX_CHON_HAM_S);
  REG32(dia_chi_iomux) = gia_tri;

  //GPIO matrix - noi thang chan voi GPIO_OUT_REG, khong qua ngoai vi nao khac
  GPIO_FUNC_OUT_SEL(chan) = MATRIX_GPIO_TRUC_TIEP | MATRIX_OE_TU_PHAN_MEM;

  //bat driver ngo ra cho chan
  REG32(GPIO_ENABLE_W1TS_REG) = (1U << chan);

  //cho tat (muc thap) luc khoi dong
  REG32(GPIO_OUT_W1TC_REG) = (1U << chan);
}

//Cau hinh chan nut nhan
static void cau_hinh_chan_nut(uint32_t chan)
{
  uint32_t dia_chi_iomux = IO_MUX_CUA_CHAN[chan];

  uint32_t gia_tri = REG32(dia_chi_iomux);
  gia_tri &= ~IOMUX_CHON_HAM_M;
  gia_tri |= (IOMUX_HAM_GPIO << IOMUX_CHON_HAM_S);
  gia_tri |= IOMUX_CHO_DOC_B;    //cho phep doc muc tin hieu tren chan
  gia_tri |= IOMUX_KEO_LEN_B;    // bat pull-up noi - nut nhan se keo xuong 0 khi bam
  gia_tri &= ~IOMUX_KEO_XUONG_B; //chac chan pull-down dang tat
  REG32(dia_chi_iomux) = gia_tri;

  REG32(GPIO_ENABLE_W1TC_REG) = (1U << chan); // dam bao chan khong bi cau hinh la ngo ra
}

//Doc muc tin hieu tho tren chan: 0 hoac 1
static inline uint32_t doc_chan(uint32_t chan)
{
  return (REG32(GPIO_IN_REG) >> chan) & 0x1U;
}

//Hien thi so tren led 7 doan

static void hien_thi_so(uint8_t so)
{
  uint8_t bit_doan = BANG_SO[so % 10U];

#if DISPLAY_COMMON_ANODE
  bit_doan = (uint8_t)(~bit_doan);  //dao bit cho man hinh common anode
#endif

  uint32_t mask_bat = 0U;
  uint32_t mask_tat = 0U;

  for (uint32_t i = 0U; i < SO_DOAN; i++) {
    uint32_t bit_chan = (1U << CHAN_DOAN[i]);
    if ((bit_doan >> i) & 0x1U) {
      mask_bat |= bit_chan;
    } else {
      mask_tat |= bit_chan;
    }
  }

  REG32(GPIO_OUT_W1TS_REG) = mask_bat;
  REG32(GPIO_OUT_W1TC_REG) = mask_tat;
}

//Debounce va nhan dang cu chi nut nhan

typedef enum {
  SU_KIEN_KHONG_CO = 0,
  SU_KIEN_NHAN,
  SU_KIEN_THA,
} su_kien_nut_t;

static uint32_t trang_thai_da_loc = 1U;   // 1 = dang tha (pull-up keo len)
static uint32_t trang_thai_tho_truoc = 1U;
static int64_t  thoi_diem_doi_muc_tho = 0;

static inline int64_t bay_gio_ms(void)
{
  return esp_timer_get_time() / 1000;
}

static su_kien_nut_t quet_nut(void)
{
  uint32_t tho = doc_chan(BTN_PIN);
  int64_t  t   = bay_gio_ms();

  if (tho != trang_thai_tho_truoc) {
    trang_thai_tho_truoc = tho;
    thoi_diem_doi_muc_tho = t;
  }

  su_kien_nut_t su_kien = SU_KIEN_KHONG_CO;

  if (tho != trang_thai_da_loc &&
      (t - thoi_diem_doi_muc_tho) >= (int64_t)DEBOUNCE_MS) {
    trang_thai_da_loc = tho;
    su_kien = (tho == 0U) ? SU_KIEN_NHAN : SU_KIEN_THA;
  }

  return su_kien;
}

static int64_t thoi_diem_bat_dau_nhan = 0;
static bool    da_kich_hoat_long_press = false;
static bool    la_ung_vien_double = false;
static int64_t thoi_diem_lap_lai_truoc = 0;

static bool    click_dang_cho = false;
static int64_t thoi_diem_click_dang_cho = 0;

static volatile uint8_t so_dang_hien = 0U;

static inline void tang_1(void) {
  so_dang_hien = (uint8_t)((so_dang_hien + 1U) % 10U);
}
static inline void giam_1(void) {
  so_dang_hien = (uint8_t)((so_dang_hien + 9U) % 10U);
}

static void xu_ly_cu_chi(void)
{
  int64_t t = bay_gio_ms();
  su_kien_nut_t su_kien = quet_nut();

  if (su_kien == SU_KIEN_NHAN) {
    thoi_diem_bat_dau_nhan = t;
    da_kich_hoat_long_press = false;

    la_ung_vien_double =
      click_dang_cho && ((t - thoi_diem_click_dang_cho) <= (int64_t)DOUBLE_CLICK_MS);
    click_dang_cho = false; // click cho da duoc lan nhan nay "nhan lay"

  } else if (su_kien == SU_KIEN_THA) {
    if (da_kich_hoat_long_press) {
      //Tha sau long press: da tinh diem qua auto-repeat roi, nen khong tinh them nua

      da_kich_hoat_long_press = false;
    } else if (la_ung_vien_double) {
      giam_1();
      la_ung_vien_double = false;
    } else {
      //Click ngan - giu cho, co the se thanh double click
      click_dang_cho = true;
      thoi_diem_click_dang_cho = t;
    }
  }

  //Trong luc dang giu nut: kiem tra nguong long press / tu dong lap
  if (trang_thai_da_loc == 0U) { //dang bi bam
    if (!da_kich_hoat_long_press &&
        (t - thoi_diem_bat_dau_nhan) >= (int64_t)LONG_PRESS_MS) {
      da_kich_hoat_long_press = true;
      la_ung_vien_double = false; //long press thi khong the la mot nua cua double click
      thoi_diem_lap_lai_truoc = t;
      tang_1();
      ESP_LOGI(TAG, "bat dau long-press -> +1 (tu dong lap)");
    } else if (da_kich_hoat_long_press &&
               (t - thoi_diem_lap_lai_truoc) >= (int64_t)REPEAT_MS) {
      thoi_diem_lap_lai_truoc = t;
      tang_1();
      ESP_LOGI(TAG, "tu dong lap -> +1");
    }
  }

  //click dang cho ma khong co click thu 2 toi kip thi +1
  if (click_dang_cho && (t - thoi_diem_click_dang_cho) >= (int64_t)DOUBLE_CLICK_MS) {
    click_dang_cho = false;
    tang_1();
    ESP_LOGI(TAG, "click don (chot tre) -> +1");
  }
}

void app_main(void)
{
  //cau hinh 7 chan doan lam ngo ra
  for (uint32_t i = 0U; i < SO_DOAN; i++) {
    cau_hinh_chan_ra(CHAN_DOAN[i]);
  }

  //cau hinh chan nut nhan
  cau_hinh_chan_nut(BTN_PIN);

  //doc truoc trang thai that de tranh nham la "vua bam" ngay luc khoi dong
  trang_thai_da_loc = trang_thai_tho_truoc = doc_chan(BTN_PIN);
  thoi_diem_doi_muc_tho = bay_gio_ms();

  hien_thi_so(0U);
  uint8_t so_da_ve = 0U;

  while (1) {
    xu_ly_cu_chi();

    if (so_dang_hien != so_da_ve) {
      so_da_ve = so_dang_hien;
      hien_thi_so(so_da_ve);
      ESP_LOGI(TAG, "so hien tai = %u", so_da_ve);
    }

    vTaskDelay(pdMS_TO_TICKS(CHU_KY_QUET_MS));
  }
}

