#include <stdio.h>

#define PI 3.14159 // Định nghĩa số PI

int main() {

  float r = 0.0f; // Khai báo bán kính

  printf("Nhap ban kinh duong tron: "); // Nhập và check
  if(scanf("%f", &r) != 1){
    return 1;
  }

  float chuvi = 2 * PI * r; // Các công thức hình học
  float dientich = PI * r * r;

  printf("Chu vi hinh tron = %.2f\nDien tich hinh tron = %.2f", chuvi, dientich);

  return 0;
}