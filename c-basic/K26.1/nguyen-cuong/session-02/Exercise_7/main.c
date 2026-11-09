#include <stdio.h>

int main() {

  float gia_hang = 0.0f, VAT = 0.0f; // khia báo biến

  printf("Nhap gia tien hang: ");
  if(scanf("%f", &gia_hang) != 1){ // nhập và check
    return 1;
  }

  printf("Nhap VAT (%): ");
  if(scanf("%f", &VAT) != 1){
    return 1;
  }

  float tien_VAT = (gia_hang * VAT) / 100;  // công thức tiền
  float sum = gia_hang + tien_VAT;

  printf("Tien VAT: %.2f\nTong tien: %.2f\n", tien_VAT, sum);

  return 0;
}