#include <stdio.h>


int main(void){
    int a,b;

    printf("Nhập số 1: ");
    scanf("%d",&a);

    printf("Nhập số 2: ");
    scanf("%d",&b);

    printf("%d + %d = %d\n",a,b,a + b);
    printf("%d - %d = %d\n",a,b,a - b);
    printf("%d / %d = %d\n",a,b,a / b);
    printf("%d * %d = %d\n",a,b,a * b);
    printf("%d %% %d = %d\n",a,b,a % b);
    
    return 0;
}