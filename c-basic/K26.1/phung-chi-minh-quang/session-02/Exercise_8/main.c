#include <stdio.h>


int main(void){
    int a = 5, b = 2;

    printf("Chia nguyên: %d / %d = %d\n",a,b,a/b);
    printf("Chia thực: (float)%d / %d = %.2f\n",a,b,(float)a/b);
    return 0;
}