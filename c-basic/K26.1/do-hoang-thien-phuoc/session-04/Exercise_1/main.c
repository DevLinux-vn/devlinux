#include <stdio.h>

int main() {
    int n;
    int count = 0; // Biến dùng để đếm số lượng

    // 1. Nhập số n
    printf("Enter n: ");
    scanf("%d", &n);

    // 2. In ra các số từ 1 đến n chia hết cho 3 hoặc 5
    printf("Numbers divisible by 3 or 5: ");
    for (int i = 1; i <= n; i++) {
        // Nếu i chia hết cho 3 HOẶC i chia hết cho 5
        if (i % 3 == 0 || i % 5 == 0) {
            printf("%d ", i);
            count++; // Mỗi lần tìm thấy 1 số thì tăng biến đếm lên 1
        }
    }
    printf("\n"); // Xuống dòng

    // 3. Đếm xem có bao nhiêu số như vậy
    printf("Total: %d numbers\n", count);

    return 0;
}