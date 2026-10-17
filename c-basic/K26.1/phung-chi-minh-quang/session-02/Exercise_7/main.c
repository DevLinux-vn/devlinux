//Exercise_7: Tính tiền VAT và tổng tiền
#include <stdio.h>

int main(void) {
    float price, vat_rate;

    printf("Nhập giá hàng: ");
    scanf("%f", &price);

    printf("Nhập VAT (%%): ");
    scanf("%f", &vat_rate);

    float vat_amount = price * vat_rate / 100;
    float total_price = price + vat_amount;

    printf("Tiền VAT: %.2f\n", vat_amount);
    printf("Tổng tiền: %.2f\n", total_price);

    return 0;
}