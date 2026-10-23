#include <stdio.h>

int main(){

    int type = 0;
    printf("Các loại hành khách:\n1. Trẻ em\n2. Người lớn\n3. Người già\nNhập số: ");
    if(scanf("%d", &type) != 1 || type > 3 || type < 1){
        printf("Nhập lỗi!\n");
        return 1;
    }

    int day = 0;
    printf("Bạn đi vào ngày (1. Thường  2. Lễ): ");
    if(scanf("%d", &day) != 1 || day > 2 || day < 1){
        printf("Nhập lỗi!\n");
        return 1;
    }

    const int fare_basic = 10000;
    float fare_change = fare_basic;

    if (day == 2) {
        fare_change = fare_change * 1.5;
    }
    
    if(type == 1){
        fare_change = fare_change * (1 - 0.3);
    }
    else if (type == 3) {
        fare_change = fare_change * (1 - 0.2);
    }

    printf("Giá vé hiện tại của bạn là: %.0f (VNĐ)", fare_change);

    return 0;
}