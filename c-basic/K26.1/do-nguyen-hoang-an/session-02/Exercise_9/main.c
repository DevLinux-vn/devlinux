/* Bài tập: Thực hiện các phép toán số học với hai số nguyên
 * Input: Hai số nguyên a và b (b phải khác 0 để thực hiện phép chia)
 * Output: Kết quả cộng, trừ, nhân, chia nguyên và chia dư
 */
#include <stdio.h>

int main()
{
	int a, b;

	printf("Nhập số 1: ");
	scanf("%d", &a);

	printf("Nhập số 2: ");
	scanf("%d", &b);

	printf("%d + %d = %d\n", a, b, a + b);
	printf("%d - %d = %d\n", a, b, a - b);
	printf("%d * %d = %d\n", a, b, a * b);

	if (b == 0)
	{
		printf("Lỗi: Không thể chia cho 0\n");
		return 1;
	}
	printf("%d / %d = %d\n", a, b, a / b);
	printf("%d %% %d = %d\n", a, b, a % b);
	return 0;
}