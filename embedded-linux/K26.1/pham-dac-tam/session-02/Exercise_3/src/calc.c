#include "calc.h"

float add_f(float a, float b) {
    return a + b;
}

float sub_f(float a, float b) {
    return a - b;
}

float mul_f(float a, float b) {
    return a * b;
}

float div_f(float a, float b) {
    if (b == 0) return -1.0;  // indicate error
    return a / b;
}
