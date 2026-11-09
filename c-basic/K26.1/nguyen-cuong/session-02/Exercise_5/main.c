#include <stdio.h>

int main() {

  float a = 0.0f, b = 0.0f; // Khai báo biến

  printf("Giai pt bac nhat: ax + b = 0\n"); // Yêu cầu giải pt bậc nhất

  printf("Nhap he so a: ");
  if(scanf("%f", &a) != 1){
    return 1;
  }

  printf("Nhap he so b: ");
  if(scanf("%f", &b) != 1){
    return 1;
  }

  if (a == 0 && b == 0) { // Xét từng trường hợp
    printf("Phuong trinh vo so nghiem!\n");
  } else if (a == 0 && b != 0) {
    printf("Phuong trinh vo nghiem!\n");
  } else if (a != 0) {
    printf("Nghiem cua phuong trinh la: %.2f\n", (-b) / a);
  }

  return 0;
}