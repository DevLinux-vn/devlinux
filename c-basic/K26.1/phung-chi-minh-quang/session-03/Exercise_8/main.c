/*Máy tính đơn giản*/
#include <stdio.h>

int main(void)
{
    float a, b;
    char c;

    printf("Nhập số 1: ");
    if (scanf("%f", &a) != 1)
    {
        printf("Lỗi: Vui lòng nhập một số hợp lệ.\n");
        return 1;
    }

    printf("Nhập toán tử: ");
    if (scanf(" %c", &c) != 1)
    {
        printf("Lỗi: Vui lòng nhập một toán tử hợp lệ.\n");
        return 1;
    }

    printf("Nhập số 2: ");
    if (scanf("%f", &b) != 1)
    {
        printf("Lỗi: Vui lòng nhập một số hợp lệ.\n");
        return 1;
    }

    // Để giữ sự đơn giản thì ở đây không check "khoảng trắng trong nhập liệu" hoặc chia cho 0

    switch (c)
    {
    case '+':
        printf("Kết quả: %.2f\n", a + b);
        break;
    case '-':
        printf("Kết quả: %.2f\n", a - b);
        break;
    case '*':
        printf("Kết quả: %.2f\n", a * b);
        break;
    case '/':
        if (b == 0)
        {
            printf("Lỗi: Không thể chia cho 0\n");
            break;
        }
        printf("Kết quả: %.2f\n", a / b);
        break;
    default:
        printf("Lỗi: Toán tử không hợp lệ\n");
        break;
    }

    return 0;
}