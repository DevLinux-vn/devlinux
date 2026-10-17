#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

int main(void)
{
    float a = 10.0;
    float b = 5.0;
    float result;

    result = calc_add(a, b);
    printf("Add: %.2f\n", result);
    log_write("Addition completed");

    result = calc_sub(a, b);
    printf("Subtract: %.2f\n", result);
    log_write("Subtraction completed");

    result = calc_mul(a, b);
    printf("Multiply: %.2f\n", result);
    log_write("Multiplication completed");

    result = calc_div(a, b);
    if (isnan(result))
    {
        printf("Division: error\n");
        log_error("Division by zero");
    }
    else
    {
        printf("Division: %.2f\n", result);
        log_write("Division completed");
    }

    result = calc_div(a, 0.0);

    if (isnan(result))
    {
        printf("Division by zero: error\n");
        log_error("Division by zero");
    }

    printf("Current time: ");
    log_timestamp();
    printf("\n");

    return 0;
}
