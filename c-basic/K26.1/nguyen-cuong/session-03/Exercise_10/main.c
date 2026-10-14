#include <stdio.h>

int main(){

    float kWh_cosume = 0.0f;
    if(scanf("%f", &kWh_cosume) != 1 || kWh_cosume < 0){
        printf("Nhập lỗi!\n");
        return 1;
    }

    int type = 0;
    printf("Loại hộ (1. Thường, 2. Chính sách): ");
    if (scanf("%d", &type) != 1 || type < 1 || type > 2) {
        printf("Loại hộ không hợp lệ!\n");
        return 1;
    }

    float total = 0.0f;
    if (kWh_cosume <= 50){
        total = kWh_cosume * 1800;
    }
    else if (kWh_cosume <= 100) {
        total = 50 * 1800 + (kWh_cosume - 50) * 2000; 
    }
    else if(kWh_cosume <= 200){
        total = (50 * 1800) + (50 * 2000) + (kWh_cosume - 100) * 2500; 
    }
    else {
        total = (50 * 1800) + (50 * 2000) + (100 * 2500) + (kWh_cosume - 200) * 3000;
    }

    if(kWh_cosume > 300 && type == 1){
        total = total * (1 + 0.05);
    }
    if(kWh_cosume <= 30 && total < 50000){
        total = 50000;
    }
    if (type == 2) {
        total = total * 0.9;
    }

    printf("Giá tiền điện của bạn là: %.0f\n", total);

    return 0;
}