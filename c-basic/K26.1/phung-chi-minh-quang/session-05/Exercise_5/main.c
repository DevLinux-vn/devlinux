// Hàm kiểm tra số đối xứng (Palindrome)
#include <stdio.h>

// Kiểm tra số nguyên dương có đối xứng hay không
int isPalindrome(int n)
{
    int original = n;
    int reversed = 0;

    while (n > 0)
    {
        reversed = reversed * 10 + n % 10; //Xây dựng số đảo ngược
        n /= 10;
    }

    return original == reversed;
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

    if (isPalindrome(n))
    {
        printf("%d là số đối xứng\n", n);
    }
    else
    {
        printf("%d không phải là số đối xứng\n", n);
    }

    return 0;
}