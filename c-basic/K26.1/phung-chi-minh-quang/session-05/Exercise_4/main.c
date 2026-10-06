// Hàm số Fibonacci
#include <stdio.h>

// Lưu ý: Thuật toán recursive có độ phức tạp O(2^n) → khác slow với n lớn.
// Cải thiện: sử dụng vòng lặp (iterative) hoặc memoization
int fibonacci(int n)
{
    if (n == 1)
        return 0;
    if (n == 2)
        return 1;

    int a = 0, b = 1;
    for (int i = 3; i <= n; i++)
    {
        int next = a + b;
        a = b;
        b = next;
    }
    return b;
}

int main(void)
{
    int n;
    printf("Nhập n: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Lỗi: Dữ liệu nhập sai\n");
        return 1;
    }
    if (n < 0)
    {
        printf("Lỗi: Vui lòng nhập số nguyên dương\n");
        return 1;
    }

    // In dãy Fibonacci từ 1 -> n bằng cách gọi hàm fibonacci() n lần
    printf("Dãy Fibonacci: ");
    for (int i = 1; i <= n; i++)
    {
        printf("%d ", fibonacci(i));
    }
    printf("\n");

    return 0;
}