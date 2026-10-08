/* Bài tập: Tính tiền lãi ngân hàng
 * Input: Số tiền gốc, lãi suất hàng năm (%) và số năm gửi
 * Output: Tiền lãi và tổng số tiền sau khi gửi
 */
#include <stdio.h>

int main()
{
	long balance, interest, total_balance, interest_rate, years;

	printf("Nhập tiền gốc: ");
	scanf("%ld", &balance);

	printf("Nhập lãi suất (%%): ");
	scanf("%ld", &interest_rate);

	printf("Nhập số năm: ");
	scanf("%ld", &years);

	// Công thức tính tiền lãi: Lãi = Số tiền gốc * Lãi suất * Số năm / 100
	interest = balance * interest_rate * years / 100;
	total_balance = balance + interest;

	printf("Tiền lãi: %ld\n", interest);
	printf("Tổng số tiền: %ld\n", total_balance);
	return 0;
}