/**
 * @file main.c
 * @brief Demonstration of IEEE-754 float traps.
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "float_inspector.h"

int main(void)
{
    printf("========== L11 Exercise 1: IEEE-754 Float Inspector ==========\n\n");

    printf("--- Bit Layout ---\n");
    inspect_float(1.0f);
    inspect_float(-2.5f);
    inspect_float(0.0f);
    inspect_float(-0.0f);
    printf("\n");

    printf("--- Special Value Classification ---\n");
    printf("1.0 / 0.0    => %s\n", classify_float(1.0f / 0.0f));
    printf("-1.0 / 0.0   => %s\n", classify_float(-1.0f / 0.0f));
    printf("0.0 / 0.0    => %s\n", classify_float(0.0f / 0.0f));
    
    /* Pre-calculate to avoid runtime domain errors during math operations */
    printf("sqrt(-1.0)   => %s\n", classify_float(sqrtf(-1.0f)));
    printf("-0.0         => %s\n", classify_float(-0.0f));
    printf("42.0         => %s\n\n", classify_float(42.0f));

    printf("--- Epsilon Comparison ---\n");
    float sum = 0.1f + 0.2f;
    float expected = 0.3f;
    printf("0.1 + 0.2 == 0.3 ? %s (raw == fails!)\n", (sum == expected) ? "YES" : "NO");
    printf("float_equal(0.1+0.2, 0.3, 1e-6) ? %s (epsilon passes)\n\n", float_equal(sum, expected, 1e-6f) ? "YES" : "NO");

    printf("--- memcmp Trap ---\n");
    float neg_zero = -0.0f;
    float pos_zero = 0.0f;
    printf("-0.0 == 0.0 ? %s (math equality)\n", (neg_zero == pos_zero) ? "YES" : "NO");
    printf("memcmp(-0.0, 0.0) == 0 ? %s (different bit patterns!)\n\n", (memcmp(&neg_zero, &pos_zero, sizeof(float)) == 0) ? "YES" : "NO");

    printf("--- Float Loop Counter Trap ---\n");
    int float_count = 0;
    for (float f = 0.0f; f < 1.0f; f += 0.1f)
    {
        float_count++;
    }
    printf("Float loop ran %d times (expected 10!)\n", float_count);

    int int_count = 0;
    for (int i = 0; i < 10; i++)
    {
        int_count++;
    }
    printf("Integer loop ran %d times (correct)\n", int_count);

    return 0;
}