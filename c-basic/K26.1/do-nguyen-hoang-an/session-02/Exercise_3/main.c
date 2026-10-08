/* Bài tập: Chuyển đổi nhiệt độ từ Celsius sang Fahrenheit
 * Input: nhiệt độ Celsius
 * Output: nhiệt độ Fahrenheit
 */
#include <stdio.h>

int main() {
	float cel, fah;

	printf("Nhập độ Celsius: ");
	scanf("%f", &cel);

	// Công thức chuyển đổi Celsius sang Fahrenheit
	// F = (C × 9/5) + 32
	fah = (cel * 9 / 5) + 32;
	printf("Độ Fahrenheit: %.2f\n", fah);

	return 0;
}