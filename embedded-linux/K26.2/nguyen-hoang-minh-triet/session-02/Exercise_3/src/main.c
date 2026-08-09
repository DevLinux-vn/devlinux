#include "calc.h"
#include "logger.h"

#include <math.h>
#include <stdio.h>

int main(void)
{
    float add_result;
    float sub_result;
    float mul_result;
    float div_result;
    char message[128];

    add_result = calc_add(10.0f, 5.0f);
    sub_result = calc_sub(10.0f, 5.0f);
    mul_result = calc_mul(10.0f, 5.0f);
    div_result = calc_div(10.0f, 5.0f);

    printf("Arithmetic Summary\n");
    printf("------------------\n");

    printf("10 + 5 = %.2f\n", add_result);
    printf("10 - 5 = %.2f\n", sub_result);
    printf("10 * 5 = %.2f\n", mul_result);
    printf("10 / 5 = %.2f\n", div_result);

    snprintf(message, sizeof(message),
             "10 + 5 = %.2f", add_result);
    log_write(message);

    snprintf(message, sizeof(message),
             "10 - 5 = %.2f", sub_result);
    log_write(message);

    snprintf(message, sizeof(message),
             "10 * 5 = %.2f", mul_result);
    log_write(message);

    snprintf(message, sizeof(message),
             "10 / 5 = %.2f", div_result);
    log_write(message);

    /* Test division by zero */
    div_result = calc_div(10.0f, 0.0f);

    if (isnan(div_result))
    {
        printf("10 / 0 = ERROR\n");
        log_error("Division by zero");
    }

    return 0;
}

