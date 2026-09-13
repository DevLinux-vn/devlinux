#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

#define MSG_SIZE 256

int main() {
    char msg[MSG_SIZE];
    float a = 10.0f, b = 5.0f, c = 0.0f;

    snprintf(msg, sizeof(msg), "calc_add: %.2f + %2.f = %.2f\n", a, b, calc_add(a, b));
    log_write(msg);
    printf("%s", msg);

    snprintf(msg, sizeof(msg), "calc_sub: %.2f - %2.f = %.2f\n", a, b, calc_sub(a, b));
    log_write(msg);
    printf("%s", msg);

    snprintf(msg, sizeof(msg), "calc_mul: %.2f * %2.f = %.2f\n", a, b, calc_mul(a, b));
    log_write(msg);
    printf("%s", msg);

    snprintf(msg, sizeof(msg), "calc_div: %.2f / %2.f = %.2f\n", a, b, calc_div(a, b));
    log_write(msg);
    printf("%s", msg);

    float div = calc_div(a, c);
    if (isnan(div)) {
        snprintf(msg, sizeof(msg), "Error - Division by zero: %.2f / %2.f = %.2f\n", a, c, calc_div(a, c));
        log_error(msg);
        printf("%s", msg);
    } else {
        log_write("Performed division");
        printf("Division result: %.2f\n", div);
    }

    log_timestamp();
    printf("Summary written to app.log\n");

    return 0;
}

