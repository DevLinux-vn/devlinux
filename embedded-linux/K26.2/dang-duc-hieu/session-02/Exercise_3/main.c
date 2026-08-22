#include "include/calc.h"
#include "include/logger.h"
#include <math.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    log_timestamp();
    log_write("Starting math and logger library test suite...");

    // Test Addition
    float num1 = 10.5f, num2 = 2.5f;
    float sum = calc_add(num1, num2);
    printf("[ADD] %.2f + %.2f = %.2f\n", num1, num2, sum);

    // Test Subtraction
    float diff = calc_sub(num1, num2);
    printf("[SUB] %.2f - %.2f = %.2f\n", num1, num2, diff);

    // Test Multiplication
    float prod = calc_mul(num1, num2);
    printf("[MUL] %.2f * %.2f = %.2f\n", num1, num2, prod);

    // Test Normal Division
    float quot = calc_div(num1, num2);
    printf("[DIV] %.2f / %.2f = %.2f\n", num1, num2, quot);

    // Test Division by Zero Edge Case
    log_write("Testing division by zero behavior...");
    float div_zero = calc_div(num1, 0.0f);

    if (isnan(div_zero))
    {
        log_error("Division by zero detected correctly (returned NAN).");
    }
    else
    {
        log_error("Division by zero failed to return NAN.");
    }

    log_timestamp();
    log_write("Test suite completed successfully.");

    return 0;
}