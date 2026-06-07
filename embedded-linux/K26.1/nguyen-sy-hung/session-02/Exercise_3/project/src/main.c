#include <stdio.h>
#include "calc.h"
#include "logger.h"

int main(void) {
    double a = 20.0;
    double b = 4.0;
    double sum;
    double diff;
    double product;
    double quotient;

    logger_log("Application started");

    sum = add(a, b);
    diff = sub(a, b);
    product = mul(a, b);
    quotient = divf(a, b);

    logger_log("Addition completed");
    logger_log("Subtraction completed");
    logger_log("Multiplication completed");
    logger_log("Division completed");

    printf("==== CALCULATION SUMMARY ====\n");
    printf("Add : %.2f\n", sum);
    printf("Subtract : %.2f\n", diff);
    printf("Multiply : %.2f\n", product);
    printf("Divide : %.2f\n", quotient);

    if (divf(a, 0.0) == 0.0) {
        logger_error("Division by zero attempted");
    }

    logger_print_timestamp();
    logger_log("Application finished");

    return 0;
}