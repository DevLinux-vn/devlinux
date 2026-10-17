#include <stdio.h>
#include <stdint.h>
#include "fixed_point.h"

#define APP_SUCCESS     (0)
#define GAIN_FACTOR     (1.75)
#define THRESHOLD       (300.0)
#define ARR_SIZE(x)     (sizeof(x) / sizeof(x[0]))
#define COMPARE(x)      (x) ? "YES" : "NO"

int32_t main() {
    int16_t samples[] = {100, 200, -150, 32767, -32768, 0};

    printf("========== L11 Exercise 2: Fixed-Point Audio DSP (Q8.8) ==========\n\n");
    printf("--- Q8.8 Gain = %.2f (Q8.8 = %d) ---\n\n", GAIN_FACTOR, (int32_t)(Q8_8_SCALE * GAIN_FACTOR));
    printf("| Original | Q8.8 Raw | Gained   | Float Result | > %.1f? |\n", THRESHOLD);
    printf("|----------|----------|----------|--------------|----------|\n");

    for(uint32_t i = 0U; i < ARR_SIZE(samples); i++) {
        if (samples[i] <= INT16_MIN || samples[i] >= INT16_MAX) {
            printf("|%-10d|  overflow|        — |            — |        — |\n", samples[i]);
        }
        else {
            int16_t float_to_q88 = float_to_q8_8(samples[i]);
            int16_t q88_to_gain = q8_8_multiply(float_to_q88, float_to_q8_8(GAIN_FACTOR));
            float q88_to_float = q8_8_to_float(q88_to_gain);
            const char *p_result = COMPARE(q8_8_exceeds_threshold(q88_to_float, THRESHOLD));

            printf("|%-10d|%-10d|%-10d|%-14.2f|%-10s|\n", samples[i], float_to_q88, q88_to_gain, q88_to_float, p_result);
        }
    }

    printf("\n--- Type Sizes ---\n");
    printf("sizeof(float)    = %zu bytes\n", sizeof(float));
    printf("sizeof(int16_t)  = %zu bytes\n", sizeof(int16_t));
    printf("Note: On Cortex-M0 without FPU, int16_t ops are native hardware instructions.\n");

    return APP_SUCCESS;
}