#include <stdio.h>
#include <math.h>

#include "calc.h"
#include "logger.h"

int main(void) {
    float a = 10.0f;
    float b = 5.0f;
    float zero = 0.0f;

    char msg[256];

    float add_result = calc_add(a, b);
    float sub_result = calc_sub(a, b);
    float mul_result = calc_mul(a, b);
    float div_result = calc_div(a, b);
    float div_zero_result = calc_div(a, zero);

    log_write("Application started");
    log_timestamp();

    snprintf(msg, sizeof(msg), "Add result: %.2f + %.2f = %.2f", a, b, add_result);
    log_write(msg);

    snprintf(msg, sizeof(msg), "Sub result: %.2f - %.2f = %.2f", a, b, sub_result);
    log_write(msg);

    snprintf(msg, sizeof(msg), "Mul result: %.2f * %.2f = %.2f", a, b, mul_result);
    log_write(msg);

    snprintf(msg, sizeof(msg), "Div result: %.2f / %.2f = %.2f", a, b, div_result);
    log_write(msg);

    if (isnan(div_zero_result)) {
        log_error("Division by zero detected");
    }

    printf("Calculation Summary\n");
    printf("-------------------\n");
    printf("Add      : %.2f\n", add_result);
    printf("Subtract : %.2f\n", sub_result);
    printf("Multiply : %.2f\n", mul_result);
    printf("Divide   : %.2f\n", div_result);
    printf("Div zero : NAN, error written to app.log\n");

    log_write("Application finished");

    return 0;
}