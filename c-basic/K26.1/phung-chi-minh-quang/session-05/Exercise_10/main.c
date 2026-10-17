#include <stdio.h>
#include <math.h>
int length(int n)
{
    int count = 0;
    while (n > 0)
    {
        count++;
        n /= 10;
    }
    return count;
}

int int_pow(int base, int exp)
{
    int result = 1;
    for (int i = 0; i < exp; i++)
    {
        result *= base;
    }
    return result;
}

int isArmstrong(int n)
{
    int len = length(n);
    int sum = 0;
    int original = n;

    while (n > 0)
    {
        int digit = n % 10;
        sum += int_pow(digit, len);
        n /= 10;
    }
    return sum == original;
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

    // In các số Armstrong bé hơn hoặc bằng n
    printf("Các số Armstrong ≤ %d: ", n);
    for (int i = 0; i <= n; i++)// Vẫn xét số 0 vì 0 cũng là số Armstrong (0^1 = 0)
    {
        if (isArmstrong(i))
        {
            printf("%d ", i);
        }
    }
    printf("\n");

    return 0;
}