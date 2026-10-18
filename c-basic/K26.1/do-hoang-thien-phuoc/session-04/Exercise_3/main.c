#include <stdio.h>

int main() {
    int n, original_n, reversed_n = 0;

    // 1. Nhập số nguyên dương n
    printf("Enter number: ");
    scanf("%d", &n);

    // Lưu lại giá trị ban đầu để in ra ở cuối cùng
    original_n = n;

    // 2. Dùng vòng lặp while để đảo ngược các chữ số
    // Lặp chừng nào n vẫn còn lớn hơn 0
    while (n > 0) {
        // Lấy chữ số cuối cùng của n
        int digit = n % 10;
        
        // Đẩy chữ số đó vào số đảo ngược (dịch số cũ sang trái 1 hàng rồi cộng số mới vào)
        reversed_n = reversed_n * 10 + digit;
        
        // Bỏ đi chữ số cuối cùng của n
        n = n / 10;
    }

    // 3. In số ban đầu và số sau khi đảo
    printf("Original number: %d\n", original_n);
    printf("Reversed number: %d\n", reversed_n);

    return 0;
}
