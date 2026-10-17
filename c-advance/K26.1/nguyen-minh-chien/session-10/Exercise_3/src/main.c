/**
 * @file    main.c
 * @brief   Demo program: safe string concatenation, bounded
 *          formatting, and the dangling pointer hazard.
 */
#include <stdio.h>
#include "safe_string.h"

/**
 * @brief   Program entry point.
 *
 * @return  0 on success.
 */
int main(void)
{
    char normal_buf[64] = "";
    char tiny_buf[7] = "";
    char sensor_buf[64];
    char truncated_sensor_buf[7];
    char prefix_buf[32];

    printf("========== L10 Lab 3: Safe String Operations ==========\n\n");

    printf("--- Safe Concatenation ---\n");
    safe_concat(normal_buf, sizeof(normal_buf), "[SYS_LOG] ");
    safe_concat(normal_buf, sizeof(normal_buf), "Voltage low.");
    printf("Normal Buffer Log: %s\n", normal_buf);

    safe_concat(tiny_buf, sizeof(tiny_buf), "[SYS_LOG] ");
    safe_concat(tiny_buf, sizeof(tiny_buf), "Voltage low.");
    printf("Tiny Buffer Log  : %s\n\n", tiny_buf);

    printf("--- Sensor Formatting ---\n");
    format_sensor_report(sensor_buf, sizeof(sensor_buf), "PRESSURE", 1024, "hPa");
    printf("Sensor Data: %s\n", sensor_buf);

    format_sensor_report(truncated_sensor_buf, sizeof(truncated_sensor_buf),
                          "PRESSURE", 1024, "hPa");
    printf("Truncated Sensor Data: %s\n\n", truncated_sensor_buf);

    printf("--- Buffer Passing Pattern ---\n");

    /* DANGER (CERT DCL30-C): the line below is intentionally left
     * commented out. get_log_prefix_BAD() returns a pointer to a
     * local stack array; by the time we would print it here, the
     * function has already returned and that memory is no longer
     * valid. Printing it would be Undefined Behavior - the output
     * could be garbage, or could even happen to look correct on one
     * run and fail on the next. NEVER do this in real code:
     *
     *   printf("Unsafe prefix: %s\n", get_log_prefix_BAD());
     */

    get_log_prefix_GOOD(prefix_buf, sizeof(prefix_buf));
    printf("Safe prefix generated: %s\n", prefix_buf);

    return 0;
}