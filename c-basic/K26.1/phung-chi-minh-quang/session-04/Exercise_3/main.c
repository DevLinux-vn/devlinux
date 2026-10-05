// Tính số đảo ngược
#include <stdio.h>

int main(void)
{
    int n;
    int reverse_n = 0;
    int last_digit;

    printf("Nhập số: ");
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

    printf("Số ban đầu: %d\n", n);

    // Lặp while để lấy số cuối cùng-last digit
    while (n > 0)
    {
        last_digit = n % 10;
        reverse_n = reverse_n * 10 + last_digit; // cộng các last digit lại để thành số ngược (reverse_n)
        n = n / 10;
    }

    printf("Số đảo ngược: %d\n", reverse_n);

    return 0;
}