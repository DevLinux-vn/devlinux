/**
 * @file main.c
 * @brief Demonstration of safe string bounds checking and memory hazard prevention.
 */
#include <stdio.h>
#include "safe_string.h"

/**
 * @brief Main execution entry point.
 * 
 * @return int Exit code.
 */
int main(void)
{
    printf("========== L10 Lab 3: Safe String Operations ==========\n\n");

    printf("--- Safe Concatenation ---\n");
    char normal_buf[64] = "[SYS_LOG] ";
    safe_concat(normal_buf, sizeof(normal_buf), "voltage low.");
    printf("Normal Buffer Log: %s\n", normal_buf);

    /* 7 bytes buffer: 5 bytes for "[SYS_", 1 char payload ('L') and 1 byte for '\0' */
    char tiny_log[7] = "[SYS_";
    safe_concat(tiny_log, sizeof(tiny_log), "LOG] voltage low.");
    printf("Tiny Buffer Log  : %s\n\n", tiny_log);

    printf("--- Sensor Formatting ---\n");
    char sensor_data[64];
    format_sensor_report(sensor_data, sizeof(sensor_data), "PRESSURE", 1024, "hPa");
    printf("Sensor Data: %s\n", sensor_data);

    /* 7 bytes buffer forces truncation to "[PRESS" (6 chars) + '\0' */
    char tiny_sensor[7];
    format_sensor_report(tiny_sensor, sizeof(tiny_sensor), "PRESSURE", 1024, "hPa");
    printf("Truncated Sensor Data: %s\n\n", tiny_sensor);

    printf("--- Buffer Passing Pattern ---\n");
    
    /* 
     * char *p_bad_ptr = get_log_prefix_BAD();
     * printf("%s\n", p_bad_ptr); 
     *
     * DANGER (Dangling Pointer): The function returns a pointer to a local stack array.
     * When get_log_prefix_BAD() exits, its memory frame is deallocated and overwritten.
     * Dereferencing p_bad_ptr accesses garbage data, causing Undefined Behavior.
     */

    char safe_prefix[32];
    get_log_prefix_GOOD(safe_prefix, sizeof(safe_prefix));
    printf("Safe prefix generated: %s\n", safe_prefix);

    return 0;
}