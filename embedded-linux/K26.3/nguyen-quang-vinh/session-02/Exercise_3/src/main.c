#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"
int main(void)
{
    float a ;
    float b ;
    printf("Nhap a: ");
    scanf("%f", &a);

    printf("Nhap b: ");
    scanf("%f", &b);

    float add_result;
    float sub_result;
    float mul_result;
    float div_result;

    char msg[100];

    /* Ghi thời gian bắt đầu chương trình */
    log_timestamp();

    /* Phép cộng */
    add_result = calc_add(a, b);
    printf("%.2f + %.2f = %.2f\n", a, b, add_result);

    snprintf(msg, sizeof(msg),
             "Addition: %.2f + %.2f = %.2f",
             a, b, add_result);

    log_write(msg);

    /* Phép trừ */
    sub_result = calc_sub(a, b);
    printf("%.2f - %.2f = %.2f\n", a, b, sub_result);

    snprintf(msg, sizeof(msg),
             "Subtraction: %.2f - %.2f = %.2f",
             a, b, sub_result);

    log_write(msg);

    /* Phép nhân */
    mul_result = calc_mul(a, b);
    printf("%.2f * %.2f = %.2f\n", a, b, mul_result);

    snprintf(msg, sizeof(msg),
             "Multiplication: %.2f * %.2f = %.2f",
             a, b, mul_result);

    log_write(msg);

    /* Phép chia */
    div_result = calc_div(a, b);

    if (isnan(div_result))
    {
        printf("Error: division by zero\n");
        log_error("Division by zero");
    }
    else
    {
        printf("%.2f / %.2f = %.2f\n", a, b, div_result);

        snprintf(msg, sizeof(msg),
                 "Division: %.2f / %.2f = %.2f",
                 a, b, div_result);

        log_write(msg);
    }

    printf("\nCalculation completed.\n");

    return 0;
}