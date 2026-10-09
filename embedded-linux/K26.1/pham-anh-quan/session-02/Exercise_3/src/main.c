#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

int main() {
    log_write("Chương trình tính toán bắt đầu khởi chạy.");

    float a = 10.5f, b = 2.5f;
    char buf[128];

    // Phép cộng
    snprintf(buf, sizeof(buf), "Thực hiện cộng: %.2f + %.2f = %.2f", a, b, calc_add(a, b));
    log_write(buf);

    // Phép trừ
    snprintf(buf, sizeof(buf), "Thực hiện trừ: %.2f - %.2f = %.2f", a, b, calc_sub(a, b));
    log_write(buf);

    // Phép nhân
    snprintf(buf, sizeof(buf), "Thực hiện nhân: %.2f * %.2f = %.2f", a, b, calc_mul(a, b));
    log_write(buf);

    // Phép chia hợp lệ
    snprintf(buf, sizeof(buf), "Thực hiện chia: %.2f / %.2f = %.2f", a, b, calc_div(a, b));
    log_write(buf);

    // Thử nghiệm chia cho 0 để kích hoạt log_error
    float zero = 0.0f;
    float div_zero_res = calc_div(a, zero);
    if (isnan(div_zero_res)) {
        log_error("Phát hiện lỗi: Chia cho số 0!");
    }

    log_write("Chương trình kết thúc thành công.");
    return 0;
}