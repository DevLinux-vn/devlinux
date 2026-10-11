#include "calc.h"
#include "logger.h"
#include <stdio.h>
#include <math.h>

int main(void){
    float a = 10.0f, b = 5.0f, result;
    char log_msg[100];

    printf("--- Calculator Application ---\n");

    printf("Current Timestamp: ");
    log_timestamp();

    result = calc_add(a, b);
    printf("\n%.2f + %.2f = %.2f\n", a, b, result);
    snprintf(log_msg, sizeof(log_msg), "Addition: result = %.2f", result);
    log_write(log_msg);

    result = calc_sub(a, b);
    printf("%.2f - %.2f = %.2f\n", a, b, result);
    snprintf(log_msg, sizeof(log_msg), "Subtraction: result = %.2f", result);
    log_write(log_msg);

    result = calc_mul(a, b);
    printf("%.2f * %.2f = %.2f\n", a, b, result);
    snprintf(log_msg, sizeof(log_msg), "Multiplication: result = %.2f", result);
    log_write(log_msg);

    result = calc_div(a, b);
    if (isnan(result)){
        printf("%.2f / %.2f = Error (division by zero)\n", a, b);
        snprintf(log_msg, sizeof(log_msg), "Division: Error (division by zero)");
        log_error(log_msg);
    } else {
        printf("%.2f / %.2f = %.2f\n", a, b, result);
        snprintf(log_msg, sizeof(log_msg), "Division: result = %.2f", result);
        log_write(log_msg);
    }

    printf("Testing division by zero:\n");
    result = calc_div(a, 0.0f);
    if (isnan(result)){
        printf("%.2f / %.2f = Error (division by zero)\n", a, 0.0f);
        log_error(log_msg);
    };


    printf("\nSummary:\n");
    printf("Arithmetic operations completed.\n");
    printf("Check app.log for log entries.\n");

    return 0;

}