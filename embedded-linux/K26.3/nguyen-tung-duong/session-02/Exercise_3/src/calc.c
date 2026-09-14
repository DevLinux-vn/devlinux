#include "calc.h"

/* Floating-point arithmetic with embedded Linux style */

float calc_add(float a, float b) {
    return a + b;
}

float calc_sub(float a, float b) {
    return a - b;
}

float calc_mul(float a, float b) {
    return a * b;
}

float calc_div(float a, float b) {
    /* Check for division by zero */
    if (b == 0.0f) {
        /* Return NAN for division by zero */
        return 0.0f / 0.0f;
    }
    return a / b;
}
