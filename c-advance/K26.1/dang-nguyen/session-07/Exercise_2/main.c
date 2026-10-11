#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#define APP_SUCCESS         (0)

#define MIN(a, b)           (((a) < (b)) ? (a) : (b))
#define MAX(a, b)           (((a) > (b)) ? (a) : (b))
#define AVG(a, b)           (((a) + (b)) / 2)

typedef struct st_stats
{
    int32_t min;
    int32_t max;
    int32_t average;
} st_stats_t;

/**
 * @brief Calculate min, max, and average of variadic integer arguments.
 * @param count Number of arguments to process.
 * @param ... Variadic int arguments (promoted from int32_t).
 * @return st_stats_t struct containing min, max, and average values.
 */
static st_stats_t compute_stats(uint32_t count, ...);

static st_stats_t compute_stats(uint32_t count, ...)
{
    st_stats_t result =
    {
        .min     = 0,
        .max     = 0,
        .average = 0
    };

    if (0U < count)
    {
        int32_t sum = 0;
        va_list ap;
        va_start(ap, count);

        for (uint32_t i = 0U; i < count; i++)
        {
            int32_t val = (int32_t)va_arg(ap, int);
            sum += val;
            
            if (0U == i)
            {
                result.min = val;
                result.max = val;
            }
            else
            {
                result.min = MIN(result.min, val);
                result.max = MAX(result.max, val);
            }
        }

        va_end(ap);

        result.average = (sum / (int32_t)count);    // NOLINT(clang-analyzer-core.DivideZero)
    }

    return result;
}

int32_t main(void)
{
    st_stats_t result;

    printf("=== Exercise 2: Variadic Stats ===\n");
    printf("Test 1 (5, 10, -5, 20, 0, 5):\n");
    result = compute_stats(5, 10, -5, 20, 0, 5);    // NOLINT(readability-magic-numbers)
    printf("Min: %d\n", result.min);
    printf("Max: %d\n", result.max);
    printf("Avg: %d\n\n", result.average);

    printf("Test 2 (2, 42, 42):\n");
    result = compute_stats(2, 42, 42);              // NOLINT(readability-magic-numbers)
    printf("Min: %d\n", result.min);
    printf("Max: %d\n", result.max);
    printf("Avg: %d\n\n", result.average);

    printf("Test 3 (0 args):\n");
    result = compute_stats(0);
    printf("Min: %d\n", result.min);
    printf("Max: %d\n", result.max);
    printf("Avg: %d\n", result.average);

    return APP_SUCCESS;
}