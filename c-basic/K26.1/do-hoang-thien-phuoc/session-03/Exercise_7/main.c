
#include <stdio.h>

int main() {
    int a, b, c;

    // Nhập vào 3 số nguyên là độ dài 3 cạnh
    printf("Nhap 3 canh: ");
    scanf("%d %d %d", &a, &b, &c);

    // Kiem tra xem 3 canh co tao thanh tam giac hop le khong
    if (a + b > c && a + c > b && b + c > a) {
        
        // Neu hop le thi bat dau phan loai:
        if (a == b && b == c) {
            printf("Tam giac deu\n");
        } 
        else if (a == b || a == c || b == c) {
            printf("Tam giac can\n");
        } 
        // Kiem tra tam giac vuong (Phai xet dinh ly Pytago cho ca 3 truong hop canh huyen)
        else if (a * a + b * b == c * c || a * a + c * c == b * b || b * b + c * c == a * a) {
            printf("Tam giac vuong\n");
        } 
        else {
            printf("Tam giac thuong\n");
        }

    } else {
        // Neu khong hop le
        printf("Khong phai tam giac hop le\n");
    }

    return 0;
}
    
