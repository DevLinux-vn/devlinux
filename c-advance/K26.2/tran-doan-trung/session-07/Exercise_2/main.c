/**
 * @file main.c
 * @brief Exercise 2: Variadic statistics computation using <stdarg.h>.
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Holds minimum, maximum, and integer average statistics.
 */
typedef struct {
    int32_t min;
    int32_t max;
    int32_t average;
} stats_t;

/**
 * @brief Computes min, max, and average of a variable number of int32_t values.
 *
 * @param[in] count Number of integer arguments following count.
 * @param[in] ...   Variable list of int arguments.
 * @return stats_t  Computed statistics, or {0, 0, 0} if count is 0.
 */
static stats_t compute_stats(uint32_t count, ...)
{
    stats_t result = {0, 0, 0};
    if (count == 0U) {
        return result;
    }

    va_list ap;
    va_start(ap, count);

    int32_t first = (int32_t)va_arg(ap, int);
    result.min = first;
    result.max = first;
    int64_t sum = first;

    for (uint32_t i = 1U; i < count; i++) {
        int32_t val = (int32_t)va_arg(ap, int);
        if (val < result.min) {
            result.min = val;
        } else if (val > result.max) {
            result.max = val;
        }
        sum += val;
    }

    va_end(ap);

    result.average = (int32_t)(sum / (int64_t)count);
    return result;
}

/**
 * @brief Entry point testing compute_stats across standard and edge cases.
 *
 * @return int Exit status (0 on success).
 */
int main(void)
{
    printf("=== Exercise 2: Variadic Stats ===\n\n");

    printf("Test 1 (5, 10, -5, 20, 0, 5):\n");
    stats_t t1 = compute_stats(5U, 10, -5, 20, 0, 5);
    printf("Min: %d\n", (int)t1.min);
    printf("Max: %d\n", (int)t1.max);
    printf("Avg: %d\n\n", (int)t1.average);

    printf("Test 2 (2, 42, 42):\n");
    stats_t t2 = compute_stats(2U, 42, 42);
    printf("Min: %d\n", (int)t2.min);
    printf("Max: %d\n", (int)t2.max);
    printf("Avg: %d\n\n", (int)t2.average);

    printf("Test 3 (0 args):\n");
    stats_t t3 = compute_stats(0U);
    printf("Min: %d\n", (int)t3.min);
    printf("Max: %d\n", (int)t3.max);
    printf("Avg: %d\n", (int)t3.average);

    return 0;
}