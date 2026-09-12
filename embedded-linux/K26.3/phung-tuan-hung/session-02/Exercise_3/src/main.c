#include <stdio.h>
#include <math.h>       /* for isnan */
#include "calc.h"
#include "logger.h"

int main(void)
{
    float a = 12.0f, b = 4.0f, c = 0.0f;
    float r;
    char buf[128];

    printf("=== Calculator with logging ===\n");
    printf("Current time: ");
    log_timestamp();

    log_write("Program started");

    /* Addition */
    r = calc_add(a, b);
    printf("%.2f + %.2f = %.2f\n", a, b, r);
    snprintf(buf, sizeof(buf), "add: %.2f + %.2f = %.2f", a, b, r);
    log_write(buf);

    /* Subtraction */
    r = calc_sub(a, b);
    printf("%.2f - %.2f = %.2f\n", a, b, r);
    snprintf(buf, sizeof(buf), "sub: %.2f - %.2f = %.2f", a, b, r);
    log_write(buf);

    /* Multiplication */
    r = calc_mul(a, b);
    printf("%.2f * %.2f = %.2f\n", a, b, r);
    snprintf(buf, sizeof(buf), "mul: %.2f * %.2f = %.2f", a, b, r);
    log_write(buf);

    /* Division (valid) */
    r = calc_div(a, b);
    printf("%.2f / %.2f = %.2f\n", a, b, r);
    snprintf(buf, sizeof(buf), "div: %.2f / %.2f = %.2f", a, b, r);
    log_write(buf);

    /* Division by zero -> NAN -> log an error */
    r = calc_div(a, c);
    if (isnan(r)) {
        printf("%.2f / %.2f = ERROR (division by zero)\n", a, c);
        snprintf(buf, sizeof(buf), "div by zero: %.2f / %.2f", a, c);
        log_error(buf);
    } else {
        printf("%.2f / %.2f = %.2f\n", a, c, r);
    }

    log_write("Program finished");
    printf("\nSummary: results written to app.log\n");
    return 0;
}
