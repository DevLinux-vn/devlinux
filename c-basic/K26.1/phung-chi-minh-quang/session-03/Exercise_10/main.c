/*Xác định giá điện*/
#include <stdio.h>

#define HO_THUONG 1
#define HO_CHINH_SACH 2

#define GIA_DIEN_BAC_1 1800
#define GIA_DIEN_BAC_2 2000
#define GIA_DIEN_BAC_3 2500
#define GIA_DIEN_BAC_4 3000

int min(int a, int b) { return (a < b) ? a : b; }
int max(int a, int b) { return (a > b) ? a : b; }

int main(void)
{
    int kWh;
    int loai_ho;
    int bac1, bac2, bac3, bac4;
    int gia_tien;

    printf("Nhập số kWh: ");
    if (scanf("%d", &kWh) != 1) {
        printf("Lỗi: Vui lòng nhập một số nguyên.\n");
        return 1;
    }

    printf("Nhập loại hộ (1=Thường, 2=Chính sách): ");
    if (scanf("%d", &loai_ho) != 1) {
        printf("Lỗi: Vui lòng nhập một số nguyên.\n");
        return 1;
    }

    // Kiểm tra input
    if (kWh < 0 || (loai_ho != HO_THUONG && loai_ho != HO_CHINH_SACH))
    {
        printf("Lỗi: Dữ liệu nhập không hợp lệ\n");
        return 1;
    }

    // Tính số tiền từng bậc

    bac1 = min(kWh, 50) * GIA_DIEN_BAC_1;// Bậc 1: 0-50 kWh
    bac2 = max(0, min(kWh, 100) - 50) * GIA_DIEN_BAC_2;// Bậc 2: 51-100 kWh
    bac3 = max(0, min(kWh, 200) - 100) * GIA_DIEN_BAC_3;// Bậc 3: 101-200 kWh
    bac4 = max(0, kWh - 200) * GIA_DIEN_BAC_4;// Bậc 4: Trên 200 kWh

    gia_tien = bac1 + bac2 + bac3 + bac4;

    // tính các trường hợp đặc biệt
    if (kWh > 300 && loai_ho == HO_THUONG)
    {
        gia_tien = (int)(gia_tien * 1.05); // phụ thu 5%
    }

    if (kWh <= 30)
    {
        gia_tien = max(gia_tien, 50000);
    }

    if (loai_ho == HO_CHINH_SACH)
    {
        gia_tien = (int)(gia_tien * 0.9);
    }

    printf("Tiền điện: %d đ\n", gia_tien);
    return 0;
}