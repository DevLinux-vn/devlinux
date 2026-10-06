//Hàm tính giai thừa
#include <stdio.h>

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

    printf("%d! = %lld\n", n, factorial(n));
    return 0;
}