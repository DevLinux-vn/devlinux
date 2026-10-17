#include "calc.h"
double addf(double a, double b) { return a + b; }
double subf(double a, double b) { return a - b; }
double mulf(double a, double b) { return a * b; }
int divf(double a, double b, double *out) {
    if (b == 0.0) return -1;
    *out = a / b;
    return 0;
}
