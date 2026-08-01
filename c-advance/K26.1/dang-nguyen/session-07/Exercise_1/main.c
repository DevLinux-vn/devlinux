#include <stdint.h>
#include <stdio.h>

#define APP_SUCCESS         (0)
#define STRINGIFY(x)        #x

#define MIN(a, b)           (((a) < (b)) ? (a) : (b))
#define ABS(x)              (((x) > 0) ? (x) : ((x) * (-1)))
#define CLAMP(val, lo, hi)      \
    do {                        \
        if ((val) < (lo))       \
        {                       \
            (val) = (lo);       \
        }                       \
        else                    \
        {                       \
            if ((val) > (hi))   \
            {                   \
                (val) = (hi);   \
            }                   \
        }                       \
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