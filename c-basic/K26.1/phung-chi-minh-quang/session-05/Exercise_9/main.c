#include <stdio.h>

#define CAL_FAC 1
#define CAL_PRI 2
#define CAL_FIB 3
#define CAL_PAL 4
#define EXIT 5

long long factorial(int n){
    if(n == 0 || n == 1){
        return 1;
    }
    long long result = 1;
    for(int i = 2; i <= n; i++){
        result *= i;
    }
    return result;
}

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

int isPalindrome(int n)
{
    int original = n;
    int reversed = 0;

    while (n > 0)
    {
        reversed = reversed * 10 + n % 10;
        n /= 10;
    }

    return original == reversed;
}

int main(void)
{
    int n;
    int choose;

    while (1)
    {
        printf("\n");
        printf("====== MENU ======\n");
        printf("1. Tính giai thừa\n");
        printf("2. Kiểm tra số nguyên tố\n");
        printf("3. Tính Fibonacci\n");
        printf("4. Kiểm tra số đối xứng\n");
        printf("5. Thoát\n");

        printf("Chọn (1-5): ");
        scanf("%d", &choose);

        // Validate choose
        if (choose == EXIT)
        {
            printf("Thoát chương trình\n");
            return 0;
        }
        if (choose < CAL_FAC || choose > EXIT)
        {
            printf("Lỗi: Lựa chọn không hợp lệ!\n");
            continue;
        }

        // Nhập n và validate n
        printf("Nhập n: ");
        if (scanf("%d", &n) != 1)
        {
            printf("Lỗi: Dữ liệu nhập sai\n");
            continue;
        }
        if (n < 0)
        {
            printf("Lỗi: Vui lòng nhập số nguyên dương\n");
            continue;
        }

        switch (choose)
        {
        case CAL_FAC:
            /* Calculate Factorial */
            printf("%d! = %lld", n, factorial(n));
            break;
        case CAL_PRI:
            /* Check if a number is Prime number */
            if (is_prime(n))
            {
                printf("%d là số nguyên tố !", n);
            }
            else
            {
                printf("%d không phải là số nguyên tố !", n);
            }
            break;
        case CAL_FIB:
            /* Tính số fibonancii */
            printf("Số fibonancy tại vị trí %d là %d", n, fibonacci(n));
            break;
        case CAL_PAL:
            /* Check if a number is palindromic-số đối xứng */
            if (isPalindrome(n))
            {
                printf("Số %d là số đối xứng", n);
            }
            else
            {
                printf("Số %d không phải là số đối xứng", n);
            }
            break;
        default:
            break;
        }
        printf("\n");
    }

    return 0;
}