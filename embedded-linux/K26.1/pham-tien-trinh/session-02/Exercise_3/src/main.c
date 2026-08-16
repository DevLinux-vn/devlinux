#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

int main(void)
{
    float a = 10.0f;
    float b = 5.0f;
    float zero = 0.0f;

    float add = calc_add(a, b);
    float sub = calc_sub(a, b);
    float mul = calc_mul(a, b);
    float div = calc_div(a, b);
    float div_zero = calc_div(a, zero);

    printf("Add: %.2f\n", add);
    printf("Sub: %.2f\n", sub);
    printf("Mul: %.2f\n", mul);
    printf("Div: %.2f\n", div);

    log_write("Program started");
    log_write("Calculated add, sub, mul, div");

    if (isnan(div_zero))
    {
        printf("Division by zero detected\n");
        log_error("Division by zero");
    }

    log_write("Program finished");

    printf("Summary printed. Check app.log\n");

    return 0;
}