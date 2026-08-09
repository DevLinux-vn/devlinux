#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

int main() {
    log_timestamp();
    log_write("Application started");

    float a = 10.5, b = 2.0;
    float result;
    char msg[100];

    // Addition
    result = calc_add(a, b);
    sprintf(msg, "Add: %.2f + %.2f = %.2f", a, b, result);
    printf("%s\n", msg);
    log_write(msg);

    // Division by zero test
    b = 0.0;
    result = calc_div(a, b);
    if (isnan(result)) {
        printf("Div: %.2f / %.2f = Error (Division by zero)\n", a, b);
        log_error("Division by zero encountered");
    }

    log_write("Application finished\n");
    return 0;
}