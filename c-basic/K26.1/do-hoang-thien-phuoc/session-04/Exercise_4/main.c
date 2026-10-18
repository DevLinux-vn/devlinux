#include <stdio.h>

int main() {
    int n;

    // 1. Nhập n
    printf("Enter n: ");
    scanf("%d", &n);

    // 2. In ra thông báo
    printf("Perfect numbers <= %d: ", n);

    // 3. Vòng lặp ngoài: duyệt từng số từ 1 đến n
    for (int i = 1; i <= n; i++) {
        int sum_of_divisors = 0; // Khởi tạo tổng các ước bằng 0 cho mỗi số i

        // 4. Vòng lặp trong: tìm và tính tổng các ước của i
        // (chỉ cần chạy đến i/2 là đủ để tìm ước lớn nhất không phải là i)
        for (int j = 1; j <= i / 2; j++) {
            if (i % j == 0) { // Nếu j là ước của i
                sum_of_divisors = sum_of_divisors + j;
            }
        }

        // 5. Kiểm tra nếu tổng các ước bằng chính số đó -> Số hoàn hảo
        if (sum_of_divisors == i && i != 0) {
            printf("%d ", i);
        }
    }

    printf("\n"); // Xuống dòng cho đẹp

    return 0;
}
