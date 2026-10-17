#include <stdio.h>

int main() {
    float F, c;

    printf("Nhap do Celsius: ");
    scanf("%f", &c);
    

    F = (c * 9.0 / 5.0) + 32;
    
    printf("Do Fahrenheit: %.2f\n", F);
    
    return 0;
}