//Hàm kiểm tra số nguyên tố

#include <stdio.h>

int is_prime(int n)
{
    if (n < 2)
        return 0; // Không phải số nguyên tố

    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
            return 0; // Không phải số nguyên tố
    }
    return 1; // Là số nguyên tố
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
    
    if (is_prime(n))
    {
        printf("%d là số nguyên tố\n", n);
    }
    else
    {
        printf("%d không phải là số nguyên tố\n", n);
    }

    return 0;
}