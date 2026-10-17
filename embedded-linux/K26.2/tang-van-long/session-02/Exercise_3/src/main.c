#include <stdio.h>
#include <math.h>

#include "calc.h"
#include "logger.h"

int main(void)
{
    float a = 10.0f;
    float b = 5.0f;

    float add_result;
    float sub_result;
    float mul_result;
    float div_result;

    char message[100];

    printf("=== Calculator Summary ===\n");

    log_timestamp();

    add_result = calc_add(a, b);
    printf("%.2f + %.2f = %.2f\n", a, b, add_result);

    snprintf(message, sizeof(message),
             "%.2f + %.2f = %.2f", a, b, add_result);
    log_write(message);

    sub_result = calc_sub(a, b);
    printf("%.2f - %.2f = %.2f\n", a, b, sub_result);

    snprintf(message, sizeof(message),
             "%.2f - %.2f = %.2f", a, b, sub_result);
    log_write(message);

    mul_result = calc_mul(a, b);
    printf("%.2f * %.2f = %.2f\n", a, b, mul_result);

    snprintf(message, sizeof(message),
             "%.2f * %.2f = %.2f", a, b, mul_result);
    log_write(message);

    div_result = calc_div(a, b);

    if (isnan(div_result))
    {
        printf("Division by zero error\n");
        log_error("Division by zero");
    }
    else
    {
        printf("%.2f / %.2f = %.2f\n", a, b, div_result);

        snprintf(message, sizeof(message),
                 "%.2f / %.2f = %.2f", a, b, div_result);
        log_write(message);
    }

    div_result = calc_div(a, 0.0f);

    if (isnan(div_result))
    {
        printf("%.2f / 0.00 = ERROR\n", a);
        log_error("Division by zero");
    }

    return 0;
}