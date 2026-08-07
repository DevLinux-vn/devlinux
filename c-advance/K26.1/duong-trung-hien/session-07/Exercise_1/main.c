#include <stdint.h>
#include <stdio.h>

#define APP_SUCCESS     (0)

/**
 * @brief Print an integer expression with its expression name.
 *
 * This macro prints the expression text and its evaluated integer value.
 * The # operator converts the expression into a string during preprocessing.
 *
 * @param expr Integer expression to evaluate and print.
 *
 * @note The expression is evaluated once.
 */
#define PRINT_INT(expr)     \
    printf("%-20s = %d\n", #expr, (expr))

/**
 * @brief Return the smaller value between two arguments.
 *
 * This macro compares two values and returns the smaller one.
 * All macro parameters and the full expression are enclosed in parentheses
 * to prevent operator precedence issues.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return The smaller value of a and b.
 */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/**
 * @brief Return the absolute value of an integer expression.
 *
 * This macro evaluates the input expression and returns its positive value.
 *
 * @param x Integer expression.
 *
 * @return Absolute value of x.
 *
 * @warning The macro argument should not contain side effects
 *          such as increment or decrement operators because the argument
 *          may be evaluated more than once.
 */
#define ABS(x)    (((x) < 0) ? -(x) : (x))

/**
 * @brief Limit a value within a specified range.
 *
 * If the value is smaller than the lower limit, it is set to the lower limit.
 * If the value is larger than the upper limit, it is set to the upper limit.
 * Otherwise, the value remains unchanged.
 *
 * This macro uses the do-while(0) pattern to ensure it behaves as a single
 * statement when used in conditional blocks.
 *
 * @param val Value to be clamped.
 * @param lo Lower limit.
 * @param hi Upper limit.
 *
 * @note The value passed through val must be a modifiable variable.
 */
#define CLAMP(val, lo, hi)          \
    do {                            \
        if ((val) < (lo))           \
        {                           \
            (val) = (lo);           \
        }                           \
        else if ((val) > (hi))      \
        {                           \
            (val) = (hi);           \
        }                           \
        else                        \
        {                           \
            (val) = (val);          \
        }                           \
    } while(0)

/**
 * @brief Test the CLAMP macro and print the result.
 *
 * This helper macro creates a temporary variable, applies CLAMP on it,
 * and prints the original input values together with the clamped result.
 *
 * @param val Input value to be tested.
 * @param lo Lower limit.
 * @param hi Upper limit.
 *
 * @note This macro is intended for testing and demonstration purposes.
 */
#define TEST_CLAMP(val, lo, hi)                  \
    do {                                         \
        int32_t x = val;                         \
        CLAMP(x, lo, hi);                        \
        printf("CLAMP(%d, %d, %d)     = %d\n",   \
                (val), (lo), (hi), (x));         \
    } while(0)

/**
 * @brief Return the smaller unsigned 32-bit integer.
 *
 * This function provides a type-safe alternative to the MIN macro.
 * Using static inline allows the compiler to optimize the function call
 * while maintaining type checking.
 *
 * @param a First unsigned 32-bit integer.
 * @param b Second unsigned 32-bit integer.
 *
 * @return The smaller value between a and b.
 */
static inline uint32_t safe_min_u32(uint32_t a, uint32_t b);

int main()
{
    printf("=== Exercise 1: Safe Macros ===\n");

    PRINT_INT(MIN(3, 5));
    PRINT_INT(ABS(-7));
    PRINT_INT(ABS(5 - 10));

    TEST_CLAMP(15, 0, 10);
    TEST_CLAMP(-3, 0, 10);

    PRINT_INT(safe_min_u32(3, 5));
    return APP_SUCCESS;
}

static inline uint32_t safe_min_u32(uint32_t a, uint32_t b)
{
    return (a < b) ? a : b;
}