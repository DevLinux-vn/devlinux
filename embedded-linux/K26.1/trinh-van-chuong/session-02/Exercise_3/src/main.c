#include <stdio.h>
#include "calc.h"
#include "logger.h"

int main() {
    printf("--- Chuong trinh tinh toan ket hop ghi Log ---\n");
    log_info("Chuong trinh bat dau chay.");

    float a = 12.5f, b = 2.5f;
    int err = 0;
    char msg_buf[100];

    // Phép cộng
    float res_add = addf(a, b);
    printf("Phep tinh: %.2f + %.2f = %.2f\n", a, b, res_add);
    sprintf(msg_buf, "Thuc hien cong: %.2f + %.2f = %.2f", a, b, res_add);
    log_info(msg_buf);

    // Phép chia chuẩn
    float res_div = divf(a, b, &err);
    printf("Phep tinh: %.2f / %.2f = %.2f\n", a, b, res_div);
    sprintf(msg_buf, "Thuc hien chia: %.2f / %.2f = %.2f", a, b, res_div);
    log_info(msg_buf);

    // Phép chia cho số 0 để test lỗi
    float c = 0.0f;
    printf("Phep tinh chi cho so tho test loi: %.2f / %.2f\n", a, c);
    divf(a, c, &err);
    if (err) {
        printf("-> Phat hien loi chia cho 0! (Xem chi tiet trong app.log)\n");
        log_error("Loi: Thuc hien phep chia cho so 0 vao bien c!");
    }

    log_info("Chuong trinh ket thuc thanh cong.");
    printf("--- Hoan thanh! Da tao file app.log ---\n");
    return 0;
}