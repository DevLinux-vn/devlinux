#include <stdio.h>

int main() {
    int n;
    float sum = 0.0;

    // 1. Nhập n
    printf("Enter n: ");
    scanf("%d", &n);

    // 2. Tính tổng và in ra chuỗi phép tính
    printf("Total: ");
    for (int i = 1; i <= n; i++) {
        // Trình bày chuỗi phép tính cho đẹp (số đầu tiên không có dấu +)
        if (i == 1) {
            printf("1");
        } else {
            printf(" + 1/%d", i);
        }
        
        // Cộng dồn vào tổng (chú ý dùng 1.0 thay vì 1)
        sum = sum + (1.0 / i);
    }

    // 3. In kết quả cuối cùng
    printf(" = %f\n", sum);

    return 0;
}