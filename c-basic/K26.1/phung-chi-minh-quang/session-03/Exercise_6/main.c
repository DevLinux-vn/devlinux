/*Xác định số ngày trong tháng*/

#include <stdio.h>

int main(void)
{
    int thang;

    printf("Nhập tháng (1-12): ");
    if (scanf("%d", &thang) != 1)
    {
        printf("Lỗi: Vui lòng nhập một số nguyên.\n");
        return 1;
    }

    if (thang < 1 || thang > 12)
    {
        printf("Lỗi: Tháng nhập vào không hợp lệ. Vui lòng nhập số từ 1 đến 12.\n");
        return 1;
    }

    switch (thang)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        printf("Tháng %d có 31 ngày\n", thang);
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        printf("Tháng %d có 30 ngày\n", thang);
        break;
    case 2:
        printf("Tháng %d có 28 ngày\n", thang);
        break;
    default:
        printf("Lỗi: Tháng nhập vào không hợp lệ. Vui lòng nhập số từ 1 đến 12.\n");
    }

    return 0;
}