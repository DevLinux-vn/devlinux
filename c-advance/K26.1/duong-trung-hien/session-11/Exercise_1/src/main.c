#include "float_inspector.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define APP_SUCCESS     (0)
#define OFFSET          (127)

#define STRINGIFY(x)    #x
#define TO_STRING(x)    STRINGIFY(x)

int32_t main() {
    printf("========== L11 Exercise 1: IEEE-754 Float Inspector ==========\n\n");

    printf("--- Bit Layout ---\n");

    float_inspector_t data;
    float value[] = {1.0f, -2.5f, 0.0f, -0.0f};
    uint8_t count = sizeof(value) / sizeof(value[0]);

    for(uint8_t i = 0U; i < count; i++) {
        inspect_float(value[i], &data);
        printf("Value: %-10.6f | Sign: %u | Exp: %-4u (Actual: %-4u) | Mantissa: 0x%06X\n", 
                value[i],
                data.bits.sign,
                data.bits.exponent,
                data.bits.exponent - 127,
                data.bits.mantissa);
    }
    printf("\n");

    printf("--- Special Value Classification ---\n");
    
    printf("%-12s => %s\n", TO_STRING(1.0f / 0.0f),     classify_float((1.0f / 0.0f), &data));
    printf("%-12s => %s\n", TO_STRING(-1.0f / 0.0f),    classify_float((-1.0f / 0.0f), &data));
    printf("%-12s => %s\n", TO_STRING(0.0f / 0.0f),     classify_float((0.0f / 0.0f), &data));
    printf("%-12s => %s\n", TO_STRING(sqrtf(-1.0f)),    classify_float(sqrtf(-1.0f), &data)); // cppcheck-suppress invalidFunctionArg
    printf("%-12s => %s\n", TO_STRING(-0.0f),           classify_float(-0.0f, &data));
    printf("%-12s => %s\n", TO_STRING(42.0f),           classify_float(42.0, &data));
    printf("\n");

    printf("--- Epsilon Comparison ---\n");
    bool result_1 = float_equal(0.1+0.2, 0.3, 1e-6);

    printf("%s ? %s (raw == fails!)\n", TO_STRING(0.1 + 0.2 == 0.3), 
                                        (0.1 + 0.2 == 0.3) ? "YES" : "NO");
    printf("%s ? %s (epsilon passes)\n\n", TO_STRING(float_equal(0.1+0.2, 0.3, 1e-6)), 
                                            (result_1) ? "YES" : "NO");

    printf("--- memcmp Trap ---\n");
    float neg_zero = -0.0;
    float pos_zero = 0.0;
    uint32_t result_2 = memcmp(&neg_zero, &pos_zero, sizeof(float));

    printf("%s ? %s (math equality)\n", TO_STRING(-0.0 == 0.0), 
                                        (-pos_zero == pos_zero) ? "YES" : "NO");
    printf("%s ? %s (different bit patterns!)\n", TO_STRING(memcmp(-0.0, 0.0, sizeof(float))),
                                                    (!result_2) ? "YES" : "NO" );
    printf("\n");

    printf("--- Float Loop Counter Trap ---\n");
    uint32_t cnt_float = 0U, cnt_int = 0U;

    for (float f = 0.0f; f < 1.0f; f += 0.1f) {
        cnt_float++;
    }
    for(uint32_t i = 0U; i < 10U; i++) {
        cnt_int++;
    }

    printf("Float loop ran %u times (expected 10!)\n", cnt_float);
    printf("Integer loop ran %u times (correct)\n", cnt_int);

    return APP_SUCCESS;
}