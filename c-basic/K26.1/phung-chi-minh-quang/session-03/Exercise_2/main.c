//Kiểm tra số âm, dương hay bằng 0
#include <stdio.h>

int main(void){
    int n;
    printf("Nhập số: ");
    
    if (scanf("%d", &n) != 1) {
        printf("Lỗi: Vui lòng nhập một số nguyên.\n");
        return 1;
    }

    if(n > 0){
        printf("%d là số dương\n",n);
    }else if(n < 0){
        printf("%d là số âm\n",n);
    }else{
        printf("%d là số bằng 0\n",n);
    }
    
    return 0;
}