#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

/**
 * @brief Stores statistical results.
 */
typedef struct
{
    int32_t min;
    int32_t max;
    int32_t average;
} stats_t;

/**
 * @brief Computes the minimum, maximum, and average of integer arguments.
 *
 * The function accepts a variable number of integer arguments. Because
 * default argument promotions apply to variadic functions, each argument
 * is retrieved as an int using va_arg() and then converted to int32_t.
 *
 * If count is zero, all fields in the returned structure are zero.
 *
 * @param count Number of integer arguments following count.
 *
 * @return stats_t Structure containing the minimum, maximum, and average.
 */
static stats_t compute_stats(uint32_t count, ...)
{
    stats_t result = {0, 0, 0};
    va_list ap;
    int64_t sum = 0;
    uint32_t index;
    int value;

    if (count == 0U)
    {
        return result;
    }

    va_start(ap, count);

    value = va_arg(ap, int);

    result.min = (int32_t)value;
    result.max = (int32_t)value;
    sum = (int64_t)value;

    for (index = 1U; index < count; ++index)
    {
        value = va_arg(ap, int);

        if ((int32_t)value < result.min)
        {
            result.min = (int32_t)value;
        }

        if ((int32_t)value > result.max)
        {
            result.max = (int32_t)value;
        }

        sum += (int64_t)value;
    }

    va_end(ap);

    result.average = (int32_t)(sum / (int64_t)count);

    return result;
}

/**
 * @brief Application entry point.
 *
 * Runs three test cases to verify the variadic statistics function.
 *
 * @return int Returns 0 when the program completes successfully.
 */
int main(void)
{
    stats_t stats;

    printf("=== Exercise 2: Variadic Stats ===\n\n");

    stats = compute_stats(5U, 10, -5, 20, 0, 5);

    printf("Test 1 (5, 10, -5, 20, 0, 5):\n");
    printf("Min: %ld\n", (long)stats.min);
    printf("Max: %ld\n", (long)stats.max);
    printf("Avg: %ld\n\n", (long)stats.average);

    stats = compute_stats(2U, 42, 42);

    printf("Test 2 (2, 42, 42):\n");
    printf("Min: %ld\n", (long)stats.min);
    printf("Max: %ld\n", (long)stats.max);
    printf("Avg: %ld\n\n", (long)stats.average);

    stats = compute_stats(0U);

    printf("Test 3 (0 args):\n");
    printf("Min: %ld\n", (long)stats.min);
    printf("Max: %ld\n", (long)stats.max);
    printf("Avg: %ld\n", (long)stats.average);

    return 0;
}