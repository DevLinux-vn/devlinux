#include <math.h>

#include "mathutils.h"

#define INT_MAX 2147483647
#define INT_MIN -2147483648
#define INT_MAX_FACTORIAL 12
#define INT_MIN_FACTORIAL 0

#define MATH_ERROR_NEGATIVE_INPUT -1
#define MATH_ERROR_OVERFLOW -2

int math_add(int a, int b) {

    return a + b;
}

int math_sub(int a, int b) {
    return a - b;
}


int math_factorial(int n) {
    // 1. Kiểm tra số âm
    if (n < 0) {
        return MATH_ERROR_NEGATIVE_INPUT; // Mã lỗi: Đầu vào âm
    }
    
    if (n > INT_MAX_FACTORIAL) {
        return MATH_ERROR_OVERFLOW; // Mã lỗi: Tràn số (Overflow - n > 12)
    }

    return n * math_factorial(n - 1);
}


