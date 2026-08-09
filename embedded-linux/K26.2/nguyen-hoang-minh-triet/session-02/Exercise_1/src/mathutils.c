#include <math.h>

#include "mathutils.h"

int math_add(int a, int b) {

    return a + b;
}

int math_sub(int a, int b) {
    return a - b;
}


int math_factorial(int n) {
    if (n < 0) {
        return -1;  // Error: negative input
    }

    if (n == 0) {
        return 1;
    }
    
    return n * math_factorial(n - 1);
}

