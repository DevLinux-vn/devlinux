#include <stdio.h>

int main() {
    double gia_tien, ti_le_vat, tien_vat, tong_tien;

    
    printf("Nhap gia tien hang: ");
    scanf("%lf", &gia_tien);

  
    printf("Nhap ti le VAT (%%): ");
    scanf("%lf", &ti_le_vat);

    
    tien_vat = gia_tien * ti_le_vat / 100;
    tong_tien = gia_tien + tien_vat;


    
    printf("Gia tien hang: %.0lf\n", gia_tien);
    printf("Tien VAT: %.0lf\n", tien_vat);
    printf("Tong tien: %.0lf\n", tong_tien);

    return 0;
}