#include <stdio.h>
#include "calc.h"
#include "logger.h"

int main(void)
{
    float a = 10.0f;
    float b = 5.0f;
    float result;

    printf("=== Calculator Summary ===\n");

    result = calc_add(a, b);
    printf("%.2f + %.2f = %.2f\n", a, b, result);
    log_write("Addition: 10.0 + 5.0 = 15.0");

    result = calc_sub(a, b);
    printf("%.2f - %.2f = %.2f\n", a, b, result);
    log_write("Subtraction: 10.0 - 5.0 = 5.0");

    result = calc_mul(a, b);
    printf("%.2f * %.2f = %.2f\n", a, b, result);
    log_write("Multiplication: 10.0 * 5.0 = 50.0");

    result = calc_div(a, b);
    printf("%.2f / %.2f = %.2f\n", a, b, result);
    log_write("Division: 10.0 / 5.0 = 2.0");

    printf("\nTesting division by zero:\n");

    if (b != 0.0f)
    {
        result = calc_div(a, b);
        printf("%.2f / %.2f = %.2f\n", a, b, result);
    }
    else
    {
        printf("Error: Division by zero\n");
        log_error("Division by zero");
    }

    printf("\nCurrent timestamp: ");
    log_timestamp();

    return 0;
}