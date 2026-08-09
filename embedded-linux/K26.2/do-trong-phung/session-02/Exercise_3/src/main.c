#include <stdio.h>
#include <math.h>

#include "calc.h"
#include "logger.h"

int main(void)
{
    float a = 20.0f;
    float b = 5.0f;
    float result;

    printf("Calculation Summary\n");
    printf("===================\n");

    result = calc_add(a, b);
    printf("Addition: %.2f + %.2f = %.2f\n", a, b, result);
    log_write("Addition operation completed");

    result = calc_sub(a, b);
    printf("Subtraction: %.2f - %.2f = %.2f\n", a, b, result);
    log_write("Subtraction operation completed");

    result = calc_mul(a, b);
    printf("Multiplication: %.2f * %.2f = %.2f\n", a, b, result);
    log_write("Multiplication operation completed");

    result = calc_div(a, b);
    if (isnan(result))
    {
        printf("Division: %.2f / %.2f = ERROR\n", a, b);
        log_error("Division by zero");
    }
    else
    {
        printf("Division: %.2f / %.2f = %.2f\n", a, b, result);
        log_write("Division operation completed");
    }

    result = calc_div(a, 0.0f);
    if (isnan(result))
    {
        printf("Division by zero: ERROR\n");
        log_error("Division by zero");
    }

    printf("\nCurrent timestamp: ");
    log_timestamp();
    printf("\n");

    return 0;
}