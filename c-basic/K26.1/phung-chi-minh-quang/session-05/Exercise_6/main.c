//Hàm tính tổng chữ số
#include <stdio.h>

int sumDigits(int n)
{
    int sum = 0;
    while (n > 0)
    {
        sum += n % 10; // Lấy chữ số cuối cùng và cộng vào tổng
        n /= 10;       // Loại bỏ chữ số cuối cùng
    }
    return sum;
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
    printf("Tổng các chữ số: %d\n", sumDigits(n));


    return 0;
}