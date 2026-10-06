#include <stdio.h>

int main() {

  float luong_brutto = 0.0f, tile_thue = 0.0f; //Khai báo biến

  printf("Nhap tien luong brutto: ");
  if(scanf("%f", &luong_brutto) != 1){
    return 1;
  }

  printf("Nhap ti le thue (%): ");
  if(scanf("%f", &tile_thue) != 1){
    return 1;
  }

  float tien_thue = (luong_brutto * tile_thue) / 100; // các công thức tính
  float luong_rong = luong_brutto - tien_thue;

  printf("Tien thue: %.2f\nLuong rong: %.2f", tien_thue, luong_rong);

  return 0;
}