#include <stdio.h>

int main() {
    int n;

    // 1. Nhập số hàng n
    printf("Enter n: ");
    scanf("%d", &n);

    // 2. Vòng lặp in tam giác Pascal
    for (int i = 0; i < n; i++) {
        
        // In khoảng trắng để tam giác cân xứng
        for (int space = 1; space <= n - i; space++) {
            printf("  "); // In 2 khoảng trắng
        }
        
        int coef = 1; // Hệ số đầu tiên của mỗi hàng luôn là 1
        
        // In các số trên hàng i
        for (int j = 0; j <= i; j++) {
            // In số hiện tại (dành 4 khoảng trống cho mỗi số để căn lề đẹp hơn)
            printf("%4d", coef);
            
            // Tính số tiếp theo trên hàng i
            coef = coef * (i - j) / (j + 1);
        }
        
        // Xuống dòng sau khi in xong một hàng
        printf("\n");
    }

    return 0;
}

        