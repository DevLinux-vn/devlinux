#include <math.h>
#include <limits.h>

#include "mathutils.h"

#define INT_MAX_FACTORIAL 12 // Giới hạn trên cho phép tính giai thừa
#define INT_MIN_FACTORIAL 0   // Giới hạn dưới cho phép tính giai thừa

#define MATH_ERROR_NEGATIVE_INPUT -1
#define MATH_ERROR_OVERFLOW -2

int math_add(int a, int b) {

    return a + b;
}

int math_sub(int a, int b) {
    return a - b;
}


int math_factorial(int n)   {
    int factorial_result;

    if (n < 0) {
        return MATH_ERROR_NEGATIVE_INPUT;
    }

    if (n > INT_MAX_FACTORIAL) {
        return MATH_ERROR_OVERFLOW;
    }

    if (n == 0) {
        return 1;
    }

    if (n == 1) {
        return 1;
    }

    factorial_result = math_factorial(n - 1);

    if (factorial_result < 0) {
        return factorial_result;
    }

    if (factorial_result > (INT_MAX / n)) {
        return MATH_ERROR_OVERFLOW;
    }

    return n * factorial_result;
}


