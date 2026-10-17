#include <stdio.h>

int main() {
    double luong_brutto, ti_le_thue, tien_thue, luong_rong;

    // 1. Nhập lương brutto
    printf("Nhap luong brutto: ");
    scanf("%lf", &luong_brutto);

    // 2. Nhập tỉ lệ thuế
    // Lưu ý: Để in ra ký tự % trên màn hình, ta phải gõ 2 dấu %%
    printf("Nhap ti le thue (%%): ");
    scanf("%lf", &ti_le_thue);

    // 3. Tính tiền thuế
    tien_thue = luong_brutto * ti_le_thue / 100;

    // 4. Tính lương ròng
    luong_rong = luong_brutto - tien_thue;

    // 5. In kết quả (Dùng %.0lf để không in ra các số 0 ở phần thập phân)
    printf("Tien thue: %.0lf\n", tien_thue);
    printf("Luong rong: %.0lf\n", luong_rong);

    return 0;
}