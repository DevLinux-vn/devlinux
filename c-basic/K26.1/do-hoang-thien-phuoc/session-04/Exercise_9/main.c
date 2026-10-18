#include <stdio.h>

int main() {
    int n;

    // 1. Nhập số n
    printf("Enter n: ");
    scanf("%d", &n);

    // 2. Vòng lặp in từ 1 đến n
    for (int i = 1; i <= n; i++) {
        // Ưu tiên kiểm tra chia hết cho cả 3 và 5 trước tiên
        if (i % 3 == 0 && i % 5 == 0) {
            printf("FizzBuzz\n");
        } 
        // Sau đó mới kiểm tra chia hết cho 3
        else if (i % 3 == 0) {
            printf("Fizz\n");
        } 
        // Và kiểm tra chia hết cho 5
        else if (i % 5 == 0) {
            printf("Buzz\n");
        } 
        // Cuối cùng, nếu không rớt vào trường hợp nào thì in chính số đó
        else {
            printf("%d\n", i);
        }
    }

    return 0;
}