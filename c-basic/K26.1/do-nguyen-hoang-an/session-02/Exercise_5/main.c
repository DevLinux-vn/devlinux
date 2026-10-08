/* Bài tập: Giải phương trình bậc nhất ax + b = 0
 * Input: Hai hệ số nguyên a và b
 * Output: Nghiệm của phương trình, vô số nghiệm hoặc vô nghiệm
 */
#include <stdio.h>

int main() {
	int a, b;

	printf("Nhập a: ");
	scanf("%d", &a);

	printf("Nhập b: ");
	scanf("%d", &b);

	// Kiểm tra các trường hợp đặc biệt của phương trình bậc 1
	if (a == 0 && b == 0) {
		printf("Vô số nghiệm\n");
	} else if (a == 0 && b != 0) {
		printf("Vô nghiệm\n");
	} else {
		printf("Nghiệm x = %.2f\n", - (float)b / a);
	}
	return 0;
}