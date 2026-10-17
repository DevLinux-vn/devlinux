#include <stdio.h>
#include "calc.h"
#include "logger.h"

int main(void) {
    log_write("Program started");

    float a = 10.0f, b = 3.0f;

    float sum = calc_add(a, b);
    float diff = calc_sub(a, b);
    float prod = calc_mul(a, b);
    float quot = calc_div(a, b);

    printf("Add: %.2f\n", sum);
    printf("Sub: %.2f\n", diff);
    printf("Mul: %.2f\n", prod);
    printf("Div: %.2f\n", quot);

    log_write("Add result logged");
    log_write("Sub result logged");
    log_write("Mul result logged");
    log_write("Div result logged");

    /* trigger and log a division-by-zero error */
    float bad = calc_div(a, 0.0f);
    printf("Div by zero: %.2f\n", bad);
    log_error("Division by zero attempted");

    log_timestamp();
    log_write("Program finished");

    printf("Summary: all operations completed. See app.log for details.\n");

    return 0;
}
