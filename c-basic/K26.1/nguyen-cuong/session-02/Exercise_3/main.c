#include <stdio.h>

int main() {

  float cel = 0.0f; // Khai báo biến nhiệt độ Cel

  printf("Nhap nhiet do Celcius: ");
  if(scanf("%f", &cel) != 1){
    return 1;
  }

  float fah = (cel * 9) / 5 + 32; // Công thức đổi
  printf("Do Fahrenheit: %.2f", fah);

  return 0;
}