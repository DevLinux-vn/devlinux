#include <stdio.h>

int main() {

  int a = 0, b = 0; // Khai báo biến để cộng

  printf("Nhap so nguyen a: "); // Nhập số
  scanf("%d", &a);

  printf("Nhap so nguyen b: ");
  scanf("%d", &b);

  int sum = a + b; // Tạo biến tổng

  printf("Tong: %d", sum);

  return 0;
}