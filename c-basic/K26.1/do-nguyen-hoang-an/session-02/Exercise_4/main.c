/* Bài tập: Tính chu vi và diện tích hình tròn
 * Input: Bán kính hình tròn
 * Output: Chu vi và diện tích hình tròn
 */
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>

int main() {
	float r;

	printf("Nhập bán kính: ");
	scanf("%f", &r);

	printf("Chu vi: %.2f\n", 2 * M_PI * r);
	printf("Diện tích: %.2f\n", M_PI * r * r);
	return 0;
}