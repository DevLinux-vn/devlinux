#include <stdio.h>

int main() {
    double kwh, tong_tien = 0;
    int loai_ho;

    // 1. Nhập số kwh tiêu thụ
    printf("Nhap so kwh tieu thu: ");
    scanf("%lf", &kwh);

    // 2. Nhập loại hộ
    printf("Nhap loai ho (1: Ho thuong, 2: Ho chinh sach): ");
    scanf("%d", &loai_ho);

    // 3. Tính tiền điện theo giá bậc thang
    if (kwh <= 50) {
        tong_tien = kwh * 1800;
    } 
    else if (kwh <= 100) {
        tong_tien = (50 * 1800) + ((kwh - 50) * 2000);
    } 
    else if (kwh <= 200) {
        tong_tien = (50 * 1800) + (50 * 2000) + ((kwh - 100) * 2500);
    } 
    else {
        tong_tien = (50 * 1800) + (50 * 2000) + (100 * 2500) + ((kwh - 200) * 3000);
    }

    // 4. Áp dụng các điều khoản
    // - Nếu kwh > 300 và hộ thường -> cộng thêm 5% phụ phí
    if (kwh > 300 && loai_ho == 1) {
        tong_tien = tong_tien + (tong_tien * 0.05);
    }

    // - Nếu hộ chính sách -> giảm 10% từ tổng tiền
    if (loai_ho == 2) {
        tong_tien = tong_tien - (tong_tien * 0.10);
    }

    // - Nếu kwh <= 30 -> tối thiểu 50,000 đ
    if (kwh <= 30 && tong_tien < 50000) {
        tong_tien = 50000;
    }

    // 5. In ra tổng tiền điện (ép kiểu .0lf để không in phần thập phân)
    printf("Tong tien dien: %.0lf d\n", tong_tien);

    return 0;
}