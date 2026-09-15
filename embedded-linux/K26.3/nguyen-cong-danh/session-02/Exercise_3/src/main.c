#include <stdio.h>
#include <math.h>

#include "calc.h"
#include "logger.h"

int main(void)
{
    float a = 20.0f;
    float b = 10.0f;

    float add_result = calc_add(a, b);
    float sub_result = calc_sub(a, b);
    float mul_result = calc_mul(a, b);
    float div_result = calc_div(a, b);
    float div_zero   = calc_div(a, 0.0f);

    char message[128];

    log_timestamp();

    snprintf(message, sizeof(message),
             "ADD: %.2f", add_result);
    log_write(message);

    snprintf(message, sizeof(message),
             "SUB: %.2f", sub_result);
    log_write(message);

    snprintf(message, sizeof(message),
             "MUL: %.2f", mul_result);
    log_write(message);

    snprintf(message, sizeof(message),
             "DIV: %.2f", div_result);
    log_write(message);

    if (isnan(div_zero))
    {
        log_error("Division by zero");
    }

    printf("====== SUMMARY ======\n");
    printf("%.2f + %.2f = %.2f\n",
           a, b, add_result);

    printf("%.2f - %.2f = %.2f\n",
           a, b, sub_result);

    printf("%.2f * %.2f = %.2f\n",
           a, b, mul_result);

    printf("%.2f / %.2f = %.2f\n",
           a, b, div_result);

    if (isnan(div_zero))
    {
        printf("Division by zero detected\n");
    }

    return 0;
}