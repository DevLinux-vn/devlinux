#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

int main() {
#define OPERAND_A 10.0f
#define OPERAND_B 2.0f
#define OPERAND_C 0.0f
    float a = OPERAND_A, b = OPERAND_B, c = OPERAND_C;
    char log_buf[256];

    log_timestamp();
    log_write("Application started");

    float add_res = calc_add(a, b);
    snprintf(log_buf, sizeof(log_buf), "Add: %.2f + %.2f = %.2f", a, b, add_res);
    log_write(log_buf);
    printf("%s\n", log_buf);

    float div_res = calc_div(a, c);
    if (isnan(div_res)) {
        snprintf(log_buf, sizeof(log_buf), "Division by zero attempted (%.2f / %.2f)", a, c);
        log_error(log_buf);
        printf("%s\n", log_buf);
    }

    log_write("Application ended");
    
    return 0;
}
