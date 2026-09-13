#include <math.h>
#include <stdio.h>

#include "calc.h"
#include "logger.h"

int main(void)
{
    float a = 10.0f;
    float b = 5.0f;

    float add_result = calc_add(a, b);
    float sub_result = calc_sub(a, b);
    float mul_result = calc_mul(a, b);
    float div_result = calc_div(a, b);
    float error_result = calc_div(a, 0.0f);

    char message[128];

    printf("Calculator Summary\n");
    printf("===================\n");

    printf("Addition:       %.2f + %.2f = %.2f\n",
           a, b, add_result);

    snprintf(message, sizeof(message),
             "Addition: %.2f + %.2f = %.2f",
             a, b, add_result);
    log_write(message);

    printf("Subtraction:    %.2f - %.2f = %.2f\n",
           a, b, sub_result);

    snprintf(message, sizeof(message),
             "Subtraction: %.2f - %.2f = %.2f",
             a, b, sub_result);
    log_write(message);

    printf("Multiplication: %.2f * %.2f = %.2f\n",
           a, b, mul_result);

    snprintf(message, sizeof(message),
             "Multiplication: %.2f * %.2f = %.2f",
             a, b, mul_result);
    log_write(message);

    printf("Division:       %.2f / %.2f = %.2f\n",
           a, b, div_result);

    snprintf(message, sizeof(message),
             "Division: %.2f / %.2f = %.2f",
             a, b, div_result);
    log_write(message);

    if (isnan(error_result))
    {
        printf("Division:       %.2f / 0.00 = ERROR\n", a);

        log_error("Division by zero");
    }

    printf("\nCurrent timestamp: ");
    log_timestamp();
    printf("\n");

    return 0;
}