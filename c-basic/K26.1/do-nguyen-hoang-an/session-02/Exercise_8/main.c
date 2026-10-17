/* Bài tập: So sánh phép chia nguyên và phép chia thực
 * Input: Hai số nguyên a = 5 và b = 2 được khai báo trong chương trình
 * Output: Kết quả chia nguyên a / b và chia thực (float)a / b
 */
#include <stdio.h>

int main()
{
	int a = 5, b = 2;

	// Chia nguyên vs chia thực - hiệu quả type casting
	printf("Chia nguyên: %d\n", a / b);
	printf("Chia thực: %.2f\n", (float)a / b);

	return 0;
}