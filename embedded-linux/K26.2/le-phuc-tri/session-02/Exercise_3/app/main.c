#include <stdio.h>
#include <math.h>

#include "calc.h"
#include "logger.h"

int main()
{
    float a = 10;
    float b = 5;

    printf("Addition       = %.2f\n", calc_add(a, b));
    printf("Subtraction    = %.2f\n", calc_sub(a, b));
    printf("Multiplication = %.2f\n", calc_mul(a, b));

    float result = calc_div(a, b);

    if (isnan(result))
    {
        printf("Division failed\n");
        log_error("Division by zero");
    }
    else
    {
        printf("Division       = %.2f\n", result);
    }

    log_write("Addition completed");
    log_write("Subtraction completed");
    log_write("Multiplication completed");
    log_write("Division completed");

    printf("Testing divide by zero...\n");

    result = calc_div(a, 0);

    if (isnan(result))
    {
        printf("Division by zero detected\n");
        log_error("Division by zero");
    }

    log_timestamp();

    return 0;
}
