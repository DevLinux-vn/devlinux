#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

int main()
{
    float a = 10;
    float b = 5;
    float result;

    result = calc_add(a, b);
    printf("Add: %.2f\n", result);
    log_timestamp();
    log_write("Addition completed");


    result = calc_sub(a, b);
    printf("Sub: %.2f\n", result);
    log_write("Subtraction completed");


    result = calc_mul(a, b);
    printf("Mul: %.2f\n", result);
    log_write("Multiplication completed");


    result = calc_div(a, 0);

    if (isnan(result))
    {
        printf("Division by zero error\n");
        log_error("Division by zero");
    }
    else
    {
        printf("Div: %.2f\n", result);
        log_write("Division completed");
    }

    return 0;
}