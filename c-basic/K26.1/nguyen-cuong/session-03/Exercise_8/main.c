#include <stdio.h>

int main(){

    float a = 0.0f, b = 0.0f;
    char toan_tu;

    printf("Nhập số đầu tiên a = ");
    if(scanf("%f", &a) != 1){
        printf("Nhập lỗi!\n");
        return 1;
    }
    
    printf("Nhập vào 1 toán tử (+, -, *, /): ");
    if(scanf("%c", &toan_tu) != 1){
        printf("Nhập toán tử lỗi\n");
        return 1;
    }

    printf("Nhập số thứ hai b = ");
    if(scanf("%f", &b) != 1){
        printf("Nhập lỗi!\n");
        return 1;
    }
    switch (toan_tu) {
        case '+':// Máy tính tự hiểu sang hệ chuyển đổi ASCII
            printf("Kết quả của phép cộng 2 số là: a + b = %.2f\n", a + b);
            break;
        case '-':
            printf("Kết quả của phép trừ là: a - b = %.2f\n", a - b);
            break;
        case '*':
            printf("Kết quả của phép nhân 2 số là: a * b = %.2f\n", a*b);
            break;
        case '/':
            if(b != 0){
                printf("Kết quả của phép chia 2 số là: a / b = %.2f\n", a/b);
            }
            else{
                printf("Phép chia không tính được khi b = 0\n");
            }
            break;
        default:
            printf("Bạn nhập không đúng toán tử!\n");
    }

    return 0;
}