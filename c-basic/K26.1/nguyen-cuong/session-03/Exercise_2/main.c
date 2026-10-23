#include <stdio.h>

int main(){

    int a = 0;
    
    printf("Nhập 1 số: ");
    if(scanf("%d", &a) != 1){
        return 1;
    }

    if(a > 0){
        printf("Số %d bạn vừa nhập là số dương!\n", a);
    }
    else if(a < 0){
        printf("Số %d bạn vừa nhập là số âm\n", a);
    }
    else{
        printf("Số bạn vừa nhập là số 0\n");
    }

    return 0;
}