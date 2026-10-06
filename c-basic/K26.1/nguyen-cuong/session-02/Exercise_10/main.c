#include <stdio.h>

int main() {

  int so_nam_gui = 0; // Khai báo các biến cần thiết
  float tien_goc = 0.0f, lai_suat = 0.0f;

  printf("Nhap tien goc (principal): "); // nhập và check
  if(scanf("%f", &tien_goc) != 1){
    return 1;
  }

  printf("Nhap lai suat (%): ");
  if(scanf("%f", &lai_suat) != 1){
    return 1;
  }

  printf("Nhap so nam muon gui: ");
  if(scanf("%d", &so_nam_gui) != 1){
    return 1;
  }

  float tien_lai = (tien_goc * lai_suat * so_nam_gui) / 100;
  float tong_tien = tien_goc + tien_lai; // các công thức tính tiền

  printf("Tien lai: %.2f\nTong tien nhan duoc: %.2f", tien_lai, tong_tien);

  return 0;
}