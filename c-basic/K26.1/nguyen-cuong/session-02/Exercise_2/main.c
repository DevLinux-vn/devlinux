#include <stdio.h>

int main() {

  float dai = 0.0f, rong = 0.0f; //Khai báo biến

  printf("Nhap chieu dai: ");
  if(scanf("%f", &dai) != 1){ //Nhập và kiểm tra biến
    return 1;
  }

  printf("Nhap chieu rong: ");
  if(scanf("%f", &rong) != 1){
    return 1;
  }

  printf("Dien tich hinh chu nhat: %.2f", dai * rong);
  return 0;
}