#include <stdio.h>
#include "calc.h"
#include "logger.h"
#include <math.h>

int main(void)
{
    float a = 10.0f;
    float b = 0.0f;
    char log_buffer[100];

    log_timestamp();
    snprintf(log_buffer, sizeof(log_buffer), "Calculating for %.2f and %.2f", a, b);
    log_write(log_buffer);

    snprintf(log_buffer, sizeof(log_buffer), "Add result: %.2f", calc_add(a, b));
    log_write(log_buffer);
    snprintf(log_buffer, sizeof(log_buffer), "Sub result: %.2f", calc_sub(a, b));
    log_write(log_buffer);
    snprintf(log_buffer, sizeof(log_buffer), "Multi result: %.2f", calc_mul(a, b));
    log_write(log_buffer);
    if (isnan(calc_div(a, b)))
    {
        log_error("Division by zero");
    }
    else
    {
        snprintf(log_buffer, sizeof(log_buffer), "Div result: %.2f", calc_div(a, b));
        log_write(log_buffer);
    }

    printf("===== Calculation Summary =====\n");
    printf("Calculating for %.2f and %.2f\n", a, b);
    printf("Add result: %.2f\n", calc_add(a, b));
    printf("Sub result: %.2f\n", calc_sub(a, b));
    printf("Multi result: %.2f\n", calc_mul(a, b));
    if (isnan(calc_div(a, b)))
    {
        printf("Division by zero\n");
    }
    else
    {
        printf("Div result: %.2f\n", calc_div(a, b));
    }

    return 0;
}