#include <stdio.h>

int main() {
    int Point;

    printf("input a point: ");
    scanf("%d", &Point);

    if(Point >= 90 && Point <= 100) {
        printf("Xuat sac\n");

    } else if(Point >= 80 && Point < 90) {
        printf("Gioi\n");

    } else if(Point >= 70 && Point < 80) {
        printf("Kha \n");

    } else if(Point >= 60 && Point < 70) {
        printf("Trung binh\n");

    } else if(Point >= 0 && Point < 60) {
        printf("Yeu\n");
        
    } else {
        printf("Invalid point\n");
    }

    
    return 0;
}