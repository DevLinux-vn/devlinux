#include <stdio.h>

int main() {
    int a, b;
    char toan_tu, tiep_tuc;

    do {
        // Nhập số thứ nhất
        printf("Enter number 1: ");
        scanf("%d", &a);

        // Nhập toán tử (có khoảng trắng trước %c để tránh trôi lệnh)
        printf("Enter operator (+, -, *, /, %%): ");
        scanf(" %c", &toan_tu);

        // Nhập số thứ hai
        printf("Enter number 2: ");
        scanf("%d", &b);

        // Dùng switch-case để thực hiện phép tính
        switch (toan_tu) {
            case '+':
                printf("Result: %d\n", a + b);
                break;
            case '-':
                printf("Result: %d\n", a - b);
                break;
            case '*':
                printf("Result: %d\n", a * b);
                break;
            case '/':
                if (b == 0) {
                    printf("Error: Division by zero!\n");
                } else {
                    printf("Result: %d\n", a / b);
                }
                break;
            case '%':
                if (b == 0) {
                    printf("Error: Modulo by zero!\n");
                } else {
                    printf("Result: %d\n", a % b);
                }
                break;
            default:
                printf("Invalid operator!\n");
                break;
        }

        // Hỏi người dùng có muốn tiếp tục không
        printf("Continue? (y/n): ");
        scanf(" %c", &tiep_tuc);
        printf("\n"); // In thêm một dòng trắng cho dễ nhìn

    } while (tiep_tuc == 'y' || tiep_tuc == 'Y'); // Lặp lại nếu người dùng nhập 'y' hoặc 'Y'

    return 0;
}