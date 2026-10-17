/* Bài tập: Tính lương ròng sau thuế
 * Input: Lương brutto và tỉ lệ thuế (%)
 * Output: Tiền thuế và lương ròng
 */
#include <stdio.h>

int main() {
	int brutto, tax_amount, netto;
	short tax_rate;

	printf("Nhập lương brutto: ");
	scanf("%d", &brutto);

	printf("Nhập tỉ lệ thuế (%%): ");
	scanf("%hd", &tax_rate);

	// Công thức tính tiền thuế: Tiền thuế = Lương brutto * Tỉ lệ thuế / 100
	tax_amount = brutto * tax_rate / 100;

	// Công thức tính lương ròng: Lương ròng = Lương brutto - Tiền thuế
	netto = brutto - tax_amount;

	printf("Tiền thuế: %d\n", tax_amount);
	printf("Lương ròng: %d\n", netto);
	return 0;
}