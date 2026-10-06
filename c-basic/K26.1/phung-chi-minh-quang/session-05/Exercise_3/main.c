#include <stdio.h>

int gcd(int a, int b)
{
    if (b == 0)
    {
        return a;
    }
    return gcd(b, a % b);
}

int lcm(int a, int b)
{
    if (a == 0 || b == 0)
        return 0;

    return (a * b) / gcd(a, b); // Để cho đơn giản, ở đây KHÔNG giải quyết trường hợp (a*b) tạo ra số lớn hơn size của integer
}

int main(void)
{
    int a, b;

    printf("Nhập số a: ");
    if (scanf("%d", &a) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }

    printf("Nhập số b: ");
    if (scanf("%d", &b) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }

    if (a <= 0 || b <= 0)
    {
        printf("Lỗi: Vui lòng nhập số nguyên dương\n");
        return 1;
    }

    printf("UCLN(%d, %d) = %d\n", a, b, gcd(a, b));
    printf("BCNN(%d, %d) = %d\n", a, b, lcm(a, b));

    return 0;
}