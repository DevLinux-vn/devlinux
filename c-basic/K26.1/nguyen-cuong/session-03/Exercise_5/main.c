#include <stdio.h>

int main(){

    int number = 0;
    const char *thu[] = {"", "thứ 2", "thứ 3", "thứ 4", "thứ 5", "thứ 6", "thứ 7", "chủ nhật"};

    printf("Nhập số (1 - 7): ");
    if(scanf("%d", &number) != 1 || number > 7 || number < 1){
        printf("Nhập lỗi!\n");
        return 1;
    }

    printf("Đó là %s\n", thu[number]);

    return 0;
}