#define _USE_MATH_DEFINES
#include<stdio.h>
#include<math.h>

int main() {
    float r,c,s;
    printf("Nhap ban kinh hinh tron: ");

    scanf("%f",&r);

    c = 2 * M_PI * r;

    s = M_PI * r * r;

    printf("Chu vi hinh tron: %.2f\n",c);

    printf("Dien tich hinh tron: %.2f\n",s);

    return 0;
}
