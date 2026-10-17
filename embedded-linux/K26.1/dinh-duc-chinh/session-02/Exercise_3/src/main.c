#include <stdio.h>
#include "calc.h"
#include "logger.h"

int main(void) {
    double r;

    r = addf(10, 5);  printf("10 + 5 = %.2f\n", r);
    log_info("addition done");

    r = mulf(3, 4);   printf("3 * 4  = %.2f\n", r);
    log_info("multiplication done");

    if (divf(10, 0, &r) != 0) {
        printf("10 / 0 = ERROR (division by zero)\n");
        log_error("division by zero attempted");
    } else {
        printf("10 / 0 = %.2f\n", r);
    }
    return 0;
}
