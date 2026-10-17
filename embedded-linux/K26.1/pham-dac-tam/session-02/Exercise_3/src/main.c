#include <stdio.h>
#include "calc.h"
#include "logger.h"

int main() {
    printf("=== Combined Static + Shared Library Demo ===\n\n");
    
    float a = 10.5, b = 3.0;
    
    printf("Operations: %.1f and %.1f\n\n", a, b);
    
    // Log timestamp
    log_timestamp();
    
    // Perform calculations
    float sum = add_f(a, b);
    float diff = sub_f(a, b);
    float prod = mul_f(a, b);
    float quot = div_f(a, b);
    
    // Print and log results
    printf("Addition: %.1f + %.1f = %.2f\n", a, b, sum);
    char buf[100];
    sprintf(buf, "ADD: %.1f + %.1f = %.2f", a, b, sum);
    log_write(buf);
    
    printf("Subtraction: %.1f - %.1f = %.2f\n", a, b, diff);
    sprintf(buf, "SUB: %.1f - %.1f = %.2f", a, b, diff);
    log_write(buf);
    
    printf("Multiplication: %.1f * %.1f = %.2f\n", a, b, prod);
    sprintf(buf, "MUL: %.1f * %.1f = %.2f", a, b, prod);
    log_write(buf);
    
    printf("Division: %.1f / %.1f = %.2f\n", a, b, quot);
    sprintf(buf, "DIV: %.1f / %.1f = %.2f", a, b, quot);
    log_write(buf);
    
    // Test division by zero
    printf("\nTesting division by zero:\n");
    float zero_div = div_f(a, 0);
    printf("%.1f / 0 = %.2f (error condition)\n", a, zero_div);
    log_error("Division by zero attempted");
    
    printf("\nResults logged to app.log\n");
    
    return 0;
}
