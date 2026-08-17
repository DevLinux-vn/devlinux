#include <stdio.h>
#include "calc.h"
#include "logger.h"

int main() {
    float a = 10.0;
    float b_valid = 2.0;
    float b_zero = 0.0;
    char log_msg[100];
    
    // Ghi log bắt đầu chương trình
    log_timestamp();
    log_write("Application started.");

    // 1. Phép cộng
    float sum = calc_add(a, b_valid);
    sprintf(log_msg, "Added %.2f + %.2f = %.2f", a, b_valid, sum);
    log_timestamp();
    log_write(log_msg);

    // 2. Phép chia hợp lệ
    float div_valid = calc_div(a, b_valid);
    sprintf(log_msg, "Divided %.2f / %.2f = %.2f", a, b_valid, div_valid);
    log_timestamp();
    log_write(log_msg);

    // 3. Phép chia cho 0
    if (b_zero == 0) {
        log_timestamp();
        log_error("Attempted to divide by zero!");
    } else {
        calc_div(a, b_zero);
    }

    log_timestamp();
    log_write("Application finished.");
    
    printf("Calculations completed. All logs have been written to 'app.log'.\n");
    printf("- Addition test: %.2f + %.2f = %.2f\n", a, b_valid, sum);
    printf("- Division test: %.2f / %.2f = %.2f\n", a, b_valid, div_valid);
    printf("- Division by zero was caught and logged.\n");

    return 0;
}