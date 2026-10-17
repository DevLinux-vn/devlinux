#include <stdio.h>

int main() {

  int a = 0, b = 0; // Khởi tạo 2 biến ban đầu

  printf("Nhap so nguyen a: "); // Nhập số
  if (scanf("%d", &a) != 1) {   // Kiểm tra có nhập đúng ko
    return 1;
  }

  printf("Nhap so nguyen b: ");
  if (scanf("%d", &b) != 1) {
    return 1;
  }

  if (b == 0) {
    printf("Không có phép chia!\nCong: %d\nTru: %d\nNhan: %d\n", a + b, a - b,
           a * b);
  } else {
    printf("Cong: %d\nTru: %d\nNhan: %d\nChia nguyen: %d\nSo du: %d\n", a + b,
           a - b, a * b, a / b, a % b);
  }

  return 0;
}