/**
 * @file main.c
 * @brief Safe Macros & Inline Functions Demonstration.
 * @details Implements parenthesized MIN, ABS, do-while(0) CLAMP macros,
 *          and type-safe static inline minimum function.
 * @author tran-doan-trung
 * @date 2026-11-27
 */

#include <stdio.h>
#include <stdint.h>

/** @brief Return the minimum of two values safely. */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/** @brief Return the absolute value of a signed integer safely. */
#define ABS(x) (((x) < 0) ? (-(x)) : (x))

/** @brief Clamp a variable in-place within [lo, hi] using do-while(0). */
#define CLAMP(val, lo, hi) \
    do { \
        if ((val) < (lo)) { \
            (val) = (lo); \
        } else if ((val) > (hi)) { \
            (val) = (hi); \
        } \
    } while (0)

/**
 * @brief Type-safe minimum using static inline.
 *
 * @param a First value.
 * @param b Second value.
 * @return The smaller of the two values.
 */
static inline uint32_t safe_min_u32(uint32_t a, uint32_t b) {
    if (a < b) {
        return a;
    }
    return b;
}

int main(void)
{
    printf("=== Exercise 1: Safe Macros ===\n");
    printf("%-20s = %d\n", "MIN(3, 5)", MIN(3, 5));
    printf("%-20s = %d\n", "ABS(-7)", ABS(-7));
    printf("%-20s = %d\n", "ABS(5 - 10)", ABS(5 - 10));
    int32_t clamp_val_1 = 15;
    int32_t clamp_val_2 = -3;
    CLAMP(clamp_val_1, 0, 10);
    CLAMP(clamp_val_2, 0, 10);
    printf("%-20s = %d\n", "CLAMP(15, 0, 10)", clamp_val_1);
    printf("%-20s = %d\n", "CLAMP(-3, 0, 10)", clamp_val_2);
    printf("%-20s = %u\n", "safe_min_u32(3, 5)", (unsigned int)safe_min_u32(3U, 5U));

    return 0;
}