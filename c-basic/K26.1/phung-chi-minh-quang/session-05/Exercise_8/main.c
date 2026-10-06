//Kiểm tra số hoàn hảo
#include <stdio.h>

int isPerfect(int n)
{
    int sum = 0;
    for (int i = 1; i < n; i++)// Tìm các ước của n, cộng vào sum, trừ chính n
    {
        if (n % i == 0)
        {
            sum = sum + i;
        }
    }
    return sum == n;
}
int main(void)
{
    int n;
    printf("Nhập n: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }
    if (n < 0)
    {
        printf("Lỗi: Vui lòng nhập số nguyên dương\n");
        return 1;
    }

    // In các số hoàn hảo bé hơn hoặc bằng n
    printf("Các số hoàn hảo ≤ %d: ", n);
    for (int i = 1; i <= n; i++)
    {
        if (isPerfect(i))
        {
            printf("%d ", i);
        }
    }
    printf("\n");

    return 0;
}