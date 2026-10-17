#include <stdio.h>
#include <stdint.h>

/**
 * @brief Returns the smaller of two values.
 *
 * @param a First value to compare. Must be side-effect free
 *          (e.g. do NOT call MIN(i++, j) — a and b are each
 *          expanded twice, so any side effect will be applied
 *          more than once).
 * @param b Second value to compare. Same side-effect warning
 *          applies as for @p a.
 *
 * @return The smaller of @p a and @p b.
 */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

/**
 * @brief find ABS(x).
 *
 * @param x if x is negative mux x with -1 else return x
 * @return ABS(x).
 */
#define ABS(x) ( ((x) < 0) ? (-(x)) : (x) )

/**
 * @def CLAMP(val, lo, hi)
 * @brief Restricts (clamps) a value to lie within [lo, hi], in place.
 *
 * @param val Lvalue (variable) to be clamped. Modified in place.
 *            Must NOT be a constant or an expression with side
 *            effects (e.g. do NOT call CLAMP(i++, 0, 10)) since
 *            val is expanded multiple times.
 * @param lo  Lower bound (inclusive).
 * @param hi  Upper bound (inclusive).
 */
#define CLAMP(val, lo, hi)                      \
        do{                                     \
            if ( (val) < (lo) )                 \
            {                                   \
                (val) = (lo);                   \
            }                                   \
            else if ( (val) > (hi) )            \
            {                                   \
                (val) = (hi);                   \
            }                                   \
            else                                \
            {                                   \
                /* val already in range, do nothing */ \
            }                                   \
        } while (0)

/**
 * @brief Type-safe minimum using static inline.
 *
 * @param a First value.
 * @param b Second value.
 * @return The smaller of the two values.
 */
static inline uint32_t safe_min_u32(uint32_t a, uint32_t b)
{
    if((a)<(b)) {
        return a;
    }else {
        return b;
    }
}

/**
 * @brief test output
 * @return 0 if successful
 */
int main(void) {
    printf("=== Exercise 1: Safe Macros ===\n");
    int8_t a = MIN(3, 5);
    int8_t b = ABS(-7);
    int8_t c = ABS(5-10);
    int8_t d = 15;
    CLAMP(d,0,10);
    int8_t e = -3;
    CLAMP(e,0,10);
    uint32_t f = safe_min_u32(3,5);
    printf("MIN(3, 5)            = %d\n", a);
    printf("ABS(-7)              = %d\n", b);
    printf("ABS(5 - 10)          = %d\n", c);
    printf("CLAMP(15, 0, 10)     = %d\n", d);
    printf("CLAMP(-3, 0, 10)     = %d\n", e);
    printf("safe_min_u32(3, 5)   = %u\n", f);
    return 0;
}

        





