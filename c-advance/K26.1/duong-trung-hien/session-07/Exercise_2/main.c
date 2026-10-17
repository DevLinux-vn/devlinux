#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#define APP_SUCCESS     (0)

/**
 * @brief Statistics result.
 *
 * Stores the minimum, maximum, and average values computed
 * from a sequence of input numbers.
 */
typedef struct
{
    int32_t min;      /**< Minimum value. */
    int32_t max;      /**< Maximum value. */
    int32_t average;  /**< Arithmetic mean of all input values. */
} stats_t;

/**
 * @brief Compute statistics from a variable number of integer arguments.
 *
 * Calculates the minimum, maximum, and arithmetic average of
 * the input values provided through a variadic argument list.
 *
 * @param[in] count Number of integer arguments to process.
 *                  If @p count is 0, all fields in the returned
 *                  structure are set to 0.
 *
 * @return A ::stats_t structure containing the computed minimum,
 *         maximum, and average values.
 *
 * @note Due to the default argument promotions in C, integer types
 *       narrower than int (such as int8_t and int16_t) are promoted
 *       to int when passed through the variadic argument list.
 *       Therefore, arguments must be retrieved using
 *       `va_arg(ap, int)` before casting to the desired type.
 */
stats_t compute_stats(uint32_t count, ...);

/**
 * @brief Print statistics to the standard output.
 *
 * @param[in] data Statistics structure containing the minimum,
 *                 maximum, and average values to print.
 */
void log_stats(stats_t data);

int main()
{
    printf("=== Exercise 2: Variadic Stats ===\n\n");

    printf("Test 1 (5, 10, -5, 20, 0, 5):\n");
    log_stats(compute_stats(5, 10, -5, 20, 0, 5));

    printf("\nTest 2 (2, 42, 42):\n");
    log_stats(compute_stats(2, 42, 42));

    printf("\nTest 3 (0 args):\n");
    log_stats(compute_stats(0));

    return APP_SUCCESS;
}

stats_t compute_stats(uint32_t count, ...)
{
    stats_t result = {0, 0, 0};
    uint32_t sample_count = count;

    if (count != 0)
    {
        int32_t sum = 0;
        va_list ap;
        va_start(ap, count);

        int32_t data = (int32_t)va_arg(ap, int);
        result.min = data;
        result.max = data;
        sum = data;

        for (uint32_t i = 1; i < sample_count; i++)
        {
            data = (int32_t)va_arg(ap, int);
            sum += data;

            if (result.min > data)
            {
                result.min = data;
            }

            if (result.max < data)
            {
                result.max = data;
            }
        }

        va_end(ap);
        result.average = sum / (int32_t)sample_count; 
    }

    return result;
}

void log_stats(stats_t data)
{
    printf("Min: %d\n", data.min);
    printf("Max: %d\n", data.max);
    printf("Avg: %d\n", data.average);
}