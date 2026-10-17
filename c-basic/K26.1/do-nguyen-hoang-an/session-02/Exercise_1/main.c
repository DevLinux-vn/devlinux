/* Bài tập: Tính tổng của hai số nguyên
 * Input: a, b là hai số nguyên
 * Output: Tổng a + b
 */
#include <stdio.h>

int main() {
	int a, b;

	printf("Nhập số thứ nhất: ");
	scanf("%d", &a);

	printf("Nhập số thứ hai: ");
	scanf("%d", &b);

	printf("Tổng: %d\n", a + b);
	return 0;
}