#include <stdio.h>

int main(){

    int month = 0;
    int day_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    printf("Nhập tháng (1 - 12): ");
    if(scanf("%d", &month) != 1 || month > 12 || month < 1){
        printf("Nhập lỗi!\n");
        return 1;
    }

    printf("Tháng bạn vừa nhập có %d ngày\n", day_in_month[month]);

    return 0;
}