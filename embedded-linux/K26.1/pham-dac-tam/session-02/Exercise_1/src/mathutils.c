#include "mathutils.h"

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

long factorial(int n) {
    if (n < 0) return -1;  // invalid input
    if (n == 0 || n == 1) return 1;
    
    long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
