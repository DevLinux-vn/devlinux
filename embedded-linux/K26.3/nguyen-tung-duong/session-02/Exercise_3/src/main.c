#include <stdio.h>
#include <string.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

int main() {
    printf("======================================\n");
    printf("Calculator with Logger\n");
    printf("======================================\n\n");
    
    /* Clear log file */
    FILE *f = fopen("app.log", "w");
    if (f) fclose(f);
    
    /* Test 1: Addition */
    log_timestamp();
    float a = 10.5f, b = 3.2f;
    float result1 = calc_add(a, b);
    printf("[1] Addition: %.2f + %.2f = %.2f\n", a, b, result1);
    
    char msg[256];
    snprintf(msg, sizeof(msg), "Addition: %.2f + %.2f = %.2f", a, b, result1);
    log_write(msg);
    
    /* Test 2: Subtraction */
    float result2 = calc_sub(a, b);
    printf("[2] Subtraction: %.2f - %.2f = %.2f\n", a, b, result2);
    snprintf(msg, sizeof(msg), "Subtraction: %.2f - %.2f = %.2f", a, b, result2);
    log_write(msg);
    
    /* Test 3: Multiplication */
    float result3 = calc_mul(a, b);
    printf("[3] Multiplication: %.2f * %.2f = %.2f\n", a, b, result3);
    snprintf(msg, sizeof(msg), "Multiplication: %.2f * %.2f = %.2f", a, b, result3);
    log_write(msg);
    
    /* Test 4: Division */
    float result4 = calc_div(a, b);
    printf("[4] Division: %.2f / %.2f = %.2f\n", a, b, result4);
    snprintf(msg, sizeof(msg), "Division: %.2f / %.2f = %.2f", a, b, result4);
    log_write(msg);
    
    /* Test 5: Division by zero */
    printf("\n[5] Division by zero test:\n");
    float result5 = calc_div(10.0f, 0.0f);
    if (isnan(result5)) {
        printf("    Result: NAN (as expected)\n");
        log_error("Division by zero attempted: 10.0 / 0.0");
    }
    
    printf("\n======================================\n");
    printf("✓ All operations completed\n");
    printf("✓ Check 'app.log' for detailed logs\n");
    printf("======================================\n");
    
    return 0;
}
