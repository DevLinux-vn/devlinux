#include "mathutils.h"

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }

unsigned long factorial(int n) {
    if (n <= 1) return 1;
    unsigned long r = 1;
    for (int i = 2; i <= n; i++) r *= i;
    return r;
}
