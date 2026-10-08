/* Bài tập: Tính tiền VAT và tổng tiền thanh toán
 * Input: Giá hàng và tỉ lệ VAT (%)
 * Output: Tiền VAT và tổng tiền thanh toán
 */
#include <stdio.h>

int main() {
	double price, vat_amount, total_price;
	short vat_rate;

	printf("Nhập giá hàng: ");
	scanf("%lf", &price);

	printf("Nhập VAT (%%): ");
	scanf("%hd", &vat_rate);

	// Công thức tính VAT: VAT = Giá hàng * Tỉ lệ VAT / 100
	vat_amount = price * vat_rate / 100;

	// Công thức tính tổng tiền: Tổng tiền = Giá hàng + VAT
	total_price = price + vat_amount;

	printf("Tiền VAT: %.0f\n", vat_amount);
	printf("Tổng tiền: %.0f\n", total_price);
	return 0;
}