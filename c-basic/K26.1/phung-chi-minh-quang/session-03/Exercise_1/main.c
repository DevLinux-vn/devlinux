//Kiểm tra số chẵn hay lẻ
#include <stdio.h>

int main(void){
    int n;

    printf("Nhập số: ");
    if(scanf("%d",&n) != 1) {
        printf("Lỗi: Vui lòng nhập một số nguyên.\n");
        return 1;
    }

    if(n % 2 == 0){
        printf("%d là số chẵn\n",n);
    }else{
        printf("%d là số lẻ\n",n);
    }
    return 0;
}