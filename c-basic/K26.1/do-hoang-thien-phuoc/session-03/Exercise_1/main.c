#include <stdio.h>

int main() {
    int a;
    
  
    printf("Nhap vao so nguyen a: ");
    scanf("%d", &a);

   
    if (a % 2 == 0) {
        printf("a is even\n");
    } else {
        printf("a is odd\n");
    }

    return 0;
}