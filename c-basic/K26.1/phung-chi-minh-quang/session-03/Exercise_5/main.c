/*Xác định thứ trong tuần*/
#include <stdio.h>

int main(void){

    int thu;

    printf("Nhập số tương ứng với thứ trong tuần (1-7): ");
    if (scanf("%d", &thu) != 1) {
        printf("Lỗi: Vui lòng nhập một số nguyên.\n");
        return 1;
    }

    if (thu < 1 || thu > 7) {
        printf("Lỗi: Số nhập vào không hợp lệ. Vui lòng nhập số từ 1 đến 7.\n");
        return 1;
    }

    switch(thu) {
        case 1:
            printf("Thứ 2\n");
            break;
        case 2:
            printf("Thứ 3\n");
            break;
        case 3:
            printf("Thứ 4\n");
            break;
        case 4:
            printf("Thứ 5\n");
            break;
        case 5:
            printf("Thứ 6\n");
            break;
        case 6:
            printf("Thứ 7\n");
            break;
        case 7:
            printf("Chủ nhật\n");
            break;
        default:
            printf("Lỗi: Số nhập vào không hợp lệ. Vui lòng nhập số từ 1 đến 7.\n");
    }

    return 0;
}