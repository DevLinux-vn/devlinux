#include <stdio.h>

typedef union{
    int i;
    float f;
    char c;
}Data;


int main(void){
    Data data;

    printf("Kích thước union: %lu bytes\n", sizeof(Data));
    data.i = 10;
    printf("data.i = %d\n", data.i);

    data.f = 3.14;
    printf("data.f = %.2f, data.i bị đổi\n", data.f);

    return 0;
}