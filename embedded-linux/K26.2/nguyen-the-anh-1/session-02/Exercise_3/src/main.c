#include <stdio.h>
#include <math.h>

#include "calc.h"
#include "logger.h"

int main(void)
{
    float a = 10.0f;
    float b = 5.0f;
    float zero = 0.0f;

    float add_result = calc_add(a, b);
    float sub_result = calc_sub(a, b);
    float mul_result = calc_mul(a, b);
    float div_result = calc_div(a, b);
    float error_result = calc_div(a, zero);

    printf("Arithmetic summary:\n");
    printf("Add: %.2f + %.2f = %.2f\n", a, b, add_result);
    printf("Sub: %.2f - %.2f = %.2f\n", a, b, sub_result);
    printf("Mul: %.2f * %.2f = %.2f\n", a, b, mul_result);

    if (!isnan(div_result))
    {
        printf("Div: %.2f / %.2f = %.2f\n", a, b, div_result);
    }

    log_write("Arithmetic operations completed.");

    if (isnan(error_result))
    {
        printf("Div: %.2f / %.2f = NAN (division by zero)\n",
               a, zero);

        log_error("Division by zero.");
    }

    return 0;
}
