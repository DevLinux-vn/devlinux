/*Xác định loại tam giác*/
#include <stdio.h>

int main(void)
{
    int a, b, c;

    printf("Nhập 3 cạnh: ");
    if (scanf("%d %d %d", &a, &b, &c) != 3)
    {
        printf("Đầu vào không hợp lệ\n");
        return 1; //Kiểm tra đầu vào có hợp lệ hay không, nếu không hợp lệ thì thoát chương trình
    }

    // Kiểm tra đây có phải tam giác hợp lệ
    // Tam giác hợp lệ khi tổng 2 cạnh bất kỳ lớn hơn cạnh còn lại và các cạnh phải lớn hơn 0
    if (a <= 0 || b <= 0 || c <= 0 || a + b <= c || a + c <= b || c + b <= a)
    {
        printf("Không phải tam giác\n");//Nếu không phải tam giác thì thoát chương trình
        return 0;
    }

    // Đặt flag kiểm tra tam giác đều/cân/vuông
    int is_deu = (a == b && a == c);
    int is_can = (a == b || a == c || b == c);
    int is_vuong = (a*a + b*b == c*c) || (a*a + c*c == b*b) || (b*b + c*c == a*a);

    // Phân loại tam giác
    if (is_deu)
    {
        printf("Đây là tam giác đều \n");
    }
    else if (is_can)
    {
        printf("Đây là tam giác cân \n");
    }
    else if (is_vuong)
    {
        printf("Đây là tam giác vuông \n");
    }
    else
    {
        printf("Đây là tam giác thường \n");
    }

    return 0;
}