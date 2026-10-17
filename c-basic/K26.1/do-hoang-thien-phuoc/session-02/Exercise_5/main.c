#include <stdio.h>

int main() {
    double a, x, b;
    printf("Nhap a: ");
    scanf("%lf", &a);
    printf("Nhap b: ");
    scanf("%lf", &b);

    

    if (a == 0 && b == 0) {
        printf("Phuong trinh co vo so nghiem\n");
    } else if (a == 0 && b != 0) {
        printf("Phuong trinh vo nghiem\n");
    } else {
        x = -b / a;
        printf("Phuong trinh co nghiem x = %.2lf\n", x); 
    }
    
    return 0;
}
    
     