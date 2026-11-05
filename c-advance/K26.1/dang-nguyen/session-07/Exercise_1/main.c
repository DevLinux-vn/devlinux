#include <stdint.h>
#include <stdio.h>

#define APP_SUCCESS         (0)
#define STRINGIFY(x)        #x

/**
 * @brief Return the smaller of two values.
 *
 * Compares @p a and @p b and evaluates to the smaller value.
 *
 * @param[in] a First value to compare.
 * @param[in] b Second value to compare.
 *
 * @return The smaller value between @p a and @p b.
 *
 * @warning The arguments may be evaluated more than once. Do not pass
 *          expressions with side effects, such as @c index++ or function calls.
 */
#define MIN(a, b)           (((a) < (b)) ? (a) : (b))

/**
 * @brief Return the absolute value of a number.
 *
 * Evaluates to @p x when @p x is positive or zero; otherwise, evaluates
 * to the negated value of @p x.
 *
 * @param[in] x Value whose absolute value is required.
 *
 * @return The absolute value of @p x.
 *
 * @warning The argument may be evaluated more than once. Do not pass
 *          expressions with side effects.
 * @warning Applying this macro to the minimum value of a signed integer type,
 *          such as @c INT32_MIN, may cause signed integer overflow.
 */
#define ABS(x)              (((x) > 0) ? (x) : ((x) * (-1)))

/**
 * @brief Clamp a value to an inclusive range.
 *
 * Modifies @p val so that it remains within the inclusive range
 * [@p lo, @p hi]&#58;  * - If @p val is less than @p lo, it is set to @p lo.
 * - If @p val is greater than @p hi, it is set to @p hi.
 * - Otherwise, @p val remains unchanged.
 *
 * @param[in,out] val Modifiable value to clamp.
 * @param[in]     lo  Inclusive lower limit.
 * @param[in]     hi  Inclusive upper limit.
 *
 * @pre @p val must be a modifiable lvalue.
 * @pre @p lo must be less than or equal to @p hi.
 *
 * @warning The arguments may be evaluated more than once. Do not pass
 *          expressions with side effects.
 */
#define CLAMP(val, lo, hi)      \
    do {                        \
        (val) = ((val) < (lo)) ? (lo) : (((val) > (hi)) ? (hi) : (val)); \
    } while (0)
    

/**
 * @brief Type-safe minimum using static inline.
 *
 * @param a First value.
 * @param b Second value.
 * @return The smaller of the two values.
 */
static inline uint32_t safe_min_u32(uint32_t a, uint32_t b);

static inline uint32_t safe_min_u32(uint32_t a, uint32_t b)
{
    return ((a < b) ? a : b);
}

int32_t main(void)
{
    int32_t clamp_val = 0;

    printf("=== Exercise 1: Safe Macros ===\n");
    printf("MIN(3, 5) = %d\n", MIN(3, 5));
    printf("ABS(-7) = %d\n", ABS(-7));
    printf("ABS(5 - 10) = %d\n", ABS(5 - 10));

    clamp_val = 15; // NOLINT(readability-magic-numbers)
    CLAMP(clamp_val, 0, 10);
    printf("CLAMP(15, 0, 10) = %d\n", clamp_val);

    clamp_val = (-3);   // NOLINT(readability-magic-numbers)
    CLAMP(clamp_val, 0, 10);
    printf("CLAMP(-3, 0, 10) = %d\n", clamp_val);

    printf("%s(3, 5) = %u\n", STRINGIFY(safe_min_u32), safe_min_u32(3, 5)); // NOLINT(readability-magic-numbers)

    return APP_SUCCESS;
}