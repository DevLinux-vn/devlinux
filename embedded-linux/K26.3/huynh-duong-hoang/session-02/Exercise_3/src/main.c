#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

int main(void) {
    char msg[128];

    log_write("Application started");
    printf("=== Calculator Program ===\n");

    float a = 10.0f, b = 2.5f;
    float res_add = calc_add(a, b);
    float res_sub = calc_sub(a, b);
    float res_mul = calc_mul(a, b);
    float res_div = calc_div(a, b);

    snprintf(msg, sizeof(msg), "Add: %.2f + %.2f = %.2f", a, b, res_add);
    log_write(msg);
    printf("%s\n", msg);

    snprintf(msg, sizeof(msg), "Sub: %.2f - %.2f = %.2f", a, b, res_sub);
    log_write(msg);
    printf("%s\n", msg);

    snprintf(msg, sizeof(msg), "Mul: %.2f * %.2f = %.2f", a, b, res_mul);
    log_write(msg);
    printf("%s\n", msg);

    snprintf(msg, sizeof(msg), "Div: %.2f / %.2f = %.2f", a, b, res_div);
    log_write(msg);
    printf("%s\n", msg);

    /* Test division by zero */
    float res_zero = calc_div(a, 0.0f);
    if (isnan(res_zero)) {
        snprintf(msg, sizeof(msg), "Division by zero detected for input %.2f / 0.00", a);
        log_error(msg);
        printf("[ERROR] %s\n", msg);
    }

    log_write("Application finished successfully");
    printf("==========================\n");
    return 0;
}