//Máy tính đơn giản (switch-case + vòng lặp)
#include <stdio.h>

int main(void)
{
    int a, b;
    char operation;
    char is_continue;

    do
    {
        printf("Nhập số 1: ");
        if (scanf("%d", &a) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            return 1;
        }

        printf("Nhập số 2: ");
        if (scanf("%d", &b) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            return 1;
        }

        printf("Nhập toán tử: ");
        if (scanf(" %c", &operation) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            return 1;
        }

        switch (operation)
        {
        case '+':
            printf("Kết quả: %d\n", a + b);
            break;
        case '-':
            printf("Kết quả: %d\n", a - b);
            break;
        case '*':
            printf("Kết quả: %d\n", a * b);
            break;
        case '/':
            if (b == 0)
            {
                printf("Lỗi: Không thể chia cho 0\n");
                return 1;
            }
            printf("Kết quả: %.2f\n", (float)a / b);
            break;
        case '%':
            if (b == 0)
            {
                printf("Lỗi: Không thể chia cho 0\n");
                return 1;
            }
            printf("Kết quả: %d\n", a % b);
            break;
        default:
            printf("Toán tử sai\n");
            break;
        }

        //Cho phép người dùng tiếp tục hoặc thoát chương trình
        printf("Tiếp tục? (y/n): ");
        if (scanf(" %c", &is_continue) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            return 1;
        }
    } while (is_continue == 'y' || is_continue == 'Y');

    return 0;
}