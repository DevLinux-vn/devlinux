#include "calc.h"
#include "logger.h"

#include <math.h>
#include <stdio.h>

int main(void)
{
    float a = 10.0f;
    float b = 2.0f;
    float zero = 0.0f;
    float sum = calc_add(a, b);
    float difference = calc_sub(a, b);
    float product = calc_mul(a, b);
    float quotient = calc_div(a, b);
    float invalid_division = calc_div(a, zero);
    char message[128];

    log_timestamp();

    snprintf(message, sizeof(message), "%.2f + %.2f = %.2f", a, b, sum);
    log_write(message);

    snprintf(message, sizeof(message), "%.2f - %.2f = %.2f", a, b, difference);
    log_write(message);

    snprintf(message, sizeof(message), "%.2f * %.2f = %.2f", a, b, product);
    log_write(message);

    snprintf(message, sizeof(message), "%.2f / %.2f = %.2f", a, b, quotient);
    log_write(message);

    if (isnan(invalid_division))
    {
        log_error("Division by zero");
    }

    printf("Calculation summary\n");
    printf("%.2f + %.2f = %.2f\n", a, b, sum);
    printf("%.2f - %.2f = %.2f\n", a, b, difference);
    printf("%.2f * %.2f = %.2f\n", a, b, product);
    printf("%.2f / %.2f = %.2f\n", a, b, quotient);
    printf("%.2f / %.2f = NAN\n", a, zero);
    printf("Log written to app.log\n");

    return 0;
}
