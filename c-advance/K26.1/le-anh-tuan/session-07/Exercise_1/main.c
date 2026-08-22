/**
 * @file main.c
 * @brief Exercise 1 - Safe Macros.
 *
 * Demonstrates properly parenthesized macros and a type-safe
 * static inline alternative.
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Returns the smaller of two values.
 *
 * @warning Do not pass expressions with side effects such as i++ because
 *          macro parameters may be evaluated more than once.
 */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/**
 * @brief Returns the absolute value of a signed value.
 *
 * @warning Do not pass expressions with side effects such as i++ because
 *          the parameter may be evaluated more than once.
 */
#define ABS(x) (((x) < 0) ? -(x) : (x))

/**
 * @brief Restricts a value to the inclusive range [lo, hi].
 *
 * If val is below lo, lo is returned. If val is above hi, hi is returned.
 * Otherwise val is returned.
 *
 * @warning Macro parameters may be evaluated more than once. Do not pass
 *          expressions with side effects.
 */
#define CLAMP(val, lo, hi) \
    (((val) < (lo)) ? (lo) : (((val) > (hi)) ? (hi) : (val)))

/**
 * @brief Returns the smaller of two uint32_t values.
 *
 * This static inline function is a type-safe alternative to the MIN macro
 * and evaluates each argument exactly once.
 *
 * @param a First unsigned 32-bit value.
 * @param b Second unsigned 32-bit value.
 *
 * @return The smaller of a and b.
 */
static inline uint32_t safe_min_u32(uint32_t a, uint32_t b)
{
    return (a < b) ? a : b;
}

/**
 * @brief Program entry point.
 *
 * Demonstrates MIN, ABS, CLAMP, and safe_min_u32 using the values
 * specified by the exercise.
 *
 * @return 0 on successful execution.
 */
int main(void)
{
    const int32_t min_result = MIN(INT32_C(3), INT32_C(5));
    const int32_t abs_negative = ABS(INT32_C(-7));
    const int32_t abs_expression = ABS(INT32_C(5) - INT32_C(10));
    const int32_t clamp_high = CLAMP(INT32_C(15), INT32_C(0), INT32_C(10));
    const int32_t clamp_low = CLAMP(INT32_C(-3), INT32_C(0), INT32_C(10));
    const uint32_t safe_min_result =
        safe_min_u32(UINT32_C(3), UINT32_C(5));

    (void)printf("=== Exercise 1: Safe Macros ===\n");
    (void)printf("MIN(3, 5)            = %" PRId32 "\n", min_result);
    (void)printf("ABS(-7)              = %" PRId32 "\n", abs_negative);
    (void)printf("ABS(5 - 10)          = %" PRId32 "\n", abs_expression);
    (void)printf("CLAMP(15, 0, 10)     = %" PRId32 "\n", clamp_high);
    (void)printf("CLAMP(-3, 0, 10)     = %" PRId32 "\n", clamp_low);
    (void)printf("safe_min_u32(3, 5)   = %" PRIu32 "\n", safe_min_result);

    return 0;
}