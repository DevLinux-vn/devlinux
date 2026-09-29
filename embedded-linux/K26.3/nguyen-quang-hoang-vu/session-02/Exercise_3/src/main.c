#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

int main(void) {
    float a = 10.0f, b = 3.0f;
    char msg[128];

    log_timestamp();

    float r1 = calc_add(a, b);
    sprintf(msg, "add(%.2f, %.2f) = %.2f", a, b, r1);
    log_write(msg);
    printf("%s\n", msg);

    float r2 = calc_sub(a, b);
    sprintf(msg, "sub(%.2f, %.2f) = %.2f", a, b, r2);
    log_write(msg);
    printf("%s\n", msg);

    float r3 = calc_mul(a, b);
    sprintf(msg, "mul(%.2f, %.2f) = %.2f", a, b, r3);
    log_write(msg);
    printf("%s\n", msg);

    float r4 = calc_div(a, 0.0f);
    if (isnan(r4)) {
        log_error("Division by zero!");
        printf("div(%.2f, 0) = ERROR (division by zero)\n", a);
    } else {
        sprintf(msg, "div(%.2f, 0) = %.2f", a, r4);
        log_write(msg);
        printf("%s\n", msg);
    }

    printf("\nSummary: results logged to app.log\n");
    return 0;
}
