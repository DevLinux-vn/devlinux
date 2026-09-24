#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

int main()
{
    float x = 10.5f;
    float y = 0.0f;
    float z = 2.0f;

    //Ghi log bat dau chuong trinh
    log_timestamp();
    log_write("Application started. Performing calculation\n");

    // Thuc hien tinh toan va in ra terminal
    printf("--- KET QUA TINH TOAN ---\n");
    printf("Add: %.2f + %.2f = %.2f\n", x, z, calc_add(x,z));
    printf("Sub: %.2f + %.2f = %.2f\n", x, z, calc_sub(x,z));
    printf("Mul: %.2f + %.2f = %.2f\n", x, z, calc_mul(x,z));

    float div_result = calc_div(x,y);
    if (isnan(div_result))
    {
        printf("Div: %.2f / %.2f = Error div by 0", x, y);
        log_error("Div by zero occurred!\n");
    }
    else
    {
        printf("Div: %.2f / %.2f = %.2f\n", x, y, div_result);
    }

    log_write("Appcation finished successfuly.\n");
    return 0;
}