#include <stdio.h>
#include "mathutils.h"

int main(){
    int a,b,n;
    // Nhập dữ liệu đầu vào
    printf("---Chương trình tính tổng hiệu và giai thừa---");
    printf("\nNhập vào số thứ nhất: a = "); 
    scanf("%d",&a);
    printf("Nhập vào số thứ hai: b = "); 
    scanf("%d",&b);
    printf("Nhập vào số thứ ba: n = "); 
    scanf("%d",&n);
    if (n < 0)
    {
        printf("\nKhông thể tính giai thừa của số âm.\n");
        return 1;
    }
    // thông báo kết quả
    printf("\n------------------Kết quả-------------------");
    printf("\nTổng của a và b là: %d",math_add(a,b));
    printf("\nHiệu của a và b là: %d",math_sub(a,b));
    printf("\nGiai thừa của n là: %d\n",math_factorial(n));
    return 0;
}