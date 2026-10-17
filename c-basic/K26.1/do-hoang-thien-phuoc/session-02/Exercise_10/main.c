#include <stdio.h>

int main() {
   double tien_goc, lai_suat, tien_lai, tong_tien;
    int so_nam;


    printf("Nhap so tien goc: ");
    scanf("%lf", &tien_goc);

    printf("Nhap lai suat hang nam (%%): ");
    scanf("%lf", &lai_suat);

    printf("Nhap so nam gui: ");
    scanf("%d", &so_nam);

   
    tien_lai = tien_goc * lai_suat * so_nam / 100;


    tong_tien = tien_goc + tien_lai;

 
    printf("\n--- KET QUA ---\n");
    printf("Tien goc: %.0lf\n", tien_goc);
    printf("Tien lai: %.0lf\n", tien_lai);
    printf("Tong tien: %.0lf\n", tong_tien);

    return 0;
}