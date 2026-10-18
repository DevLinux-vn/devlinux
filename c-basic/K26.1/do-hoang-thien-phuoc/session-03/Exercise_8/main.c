#include <stdio.h>

int main() {
    double so1, so2;
    char toan_tu;

    // Nhập số thứ nhất
    printf("Nhap so 1: ");
    scanf("%lf", &so1);

    // Nhập toán tử (Lưu ý khoảng trắng trước %c để tránh lỗi trôi lệnh)
    printf("Nhap toan tu (+, -, *, /): ");
    scanf(" %c", &toan_tu);

    // Nhập số thứ hai
    printf("Nhap so 2: ");
    scanf("%lf", &so2);

    // Dùng switch-case để tính toán
    switch (toan_tu) {
        case '+':
            printf("Ket qua: %.2lf\n", so1 + so2);
            break;
        case '-':
            printf("Ket qua: %.2lf\n", so1 - so2);
            break;
        case '*':
            printf("Ket qua: %.2lf\n", so1 * so2);
            break;
        case '/':
            // Kiểm tra điều kiện chia cho 0
            if (so2 == 0) {
                printf("Loi: Khong the chia cho 0!\n");
            } else {
                printf("Ket qua: %.2lf\n", so1 / so2);
            }
            break;
        default:
            // Báo lỗi nếu nhập sai toán tử
            printf("Loi: Toan tu khong hop le!\n");
            break;
    }

    return 0;
}