#include <stdio.h>
#include "mathutils.h"

int main() {
    int x, y, n;
    
    printf("--- Test Thu Vien Tinh MathUtils ---\n");
    printf("Nhap hai so nguyen de tinh Tong/Hieu (vi du: 10 5): ");
    if (scanf("%d %d", &x, &y) != 2) return 1;
    
    printf("Ket qua %d + %d = %d\n", x, y, add(x, y));
    printf("Ket qua %d - %d = %d\n", x, y, subtract(x, y));
    
    printf("\nNhap mot so nguyen khong am de tinh giai thua (vi du: 5): ");
    if (scanf("%d", &n) != 1) return 1;
    
    long long fact = factorial(n);
    if (fact == -1) {
        printf("Loi: Khong tinh duoc giai thua cua so am!\n");
    } else {
        printf("Ket qua %d! = %lld\n", n, fact);
    }
    
    return 0;
}