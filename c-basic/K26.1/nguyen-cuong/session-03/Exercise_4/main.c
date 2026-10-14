#include <stdio.h>

int main(){

    int age = 0;

    printf("Nhập số tuổi hiện tại của bạn (1 - 100): ");
    if(scanf("%d", &age) != 1 || age > 100 || age < 1){
        printf("Nhập lỗi!\n");
        return 1;
    }

    if(age >= 18){
        printf("Bạn đủ điêuf kiện thi bằng lái xe máy!\n");
    }
    else{
        printf("Bạn không đủ điều kiện!\n");
    }

    return 0;
}