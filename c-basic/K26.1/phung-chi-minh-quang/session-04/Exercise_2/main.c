// Tính tổng nghịch đảo
#include <stdio.h>

int main(void)
{
    int n;
    float sum = 1.0; // Khởi tạo tổng với giá trị 1 (tương ứng với 1/1)

    printf("Nhập n: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }

    printf("Tổng : 1 ");
    for (int i = 2; i <= n; i++)
    {
        sum += 1.0 / i;
        printf("+ 1/%d ", i);
    }

    printf("= %.4f \n", sum);
    return 0;
}