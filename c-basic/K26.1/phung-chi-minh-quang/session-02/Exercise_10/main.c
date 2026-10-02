// Exercise_10: Tính tiền lãi và tổng tiền
#include <stdio.h>

int main(void)
{
    float tien_goc, lai_suat;
    int so_nam;
    float tien_lai;
    float tong_tien;

    printf("Nhập tiền gốc: ");
    scanf("%f", &tien_goc);

    printf("Nhập lãi suất (%%): ");
    scanf("%f", &lai_suat);

    printf("Nhập số năm: ");
    scanf("%d", &so_nam);

    tien_lai = tien_goc * lai_suat * so_nam / 100;
    printf("Tiền lãi: %.2f\n", tien_lai);

    tong_tien = tien_goc + tien_lai;
    printf("Tổng tiền: %.2f\n", tong_tien);

    return 0;
}
