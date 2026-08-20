#include "safe_string.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define APP_SUCCESS         (0)

int32_t main() {
    char buf[BUFFER_SIZE] = {0};
    char tiny_buf[TINY_BUFFER_SIZE] = {0};
    const char concat_src[] = "[SYS_LOG] Voltage low.";

    printf("========== L10 Lab 3: Safe String Operations ==========\n\n");

    printf("--- Safe Concatenation ---\n");
    safe_concat(buf, BUFFER_SIZE, concat_src);
    printf("Normal Buffer Log: %s\n", buf);
    safe_concat(tiny_buf, TINY_BUFFER_SIZE, concat_src);
    printf("Tiny Buffer Log  : %s\n\n", tiny_buf);

    printf("--- Sensor Formatting ---\n");

    memset(buf, 0, BUFFER_SIZE);
    memset(tiny_buf, 0, TINY_BUFFER_SIZE);

    format_sensor_report(buf, BUFFER_SIZE, "PRESSURE", 1024, "hPa");
    printf("Sensor Data: %s\n", buf);
    format_sensor_report(tiny_buf, TINY_BUFFER_SIZE, "PRESSURE", 1024, "hPa");
    printf("Truncated Sensor Data: %s\n\n", tiny_buf);

    printf("--- Buffer Passing Pattern ---\n");
    
    memset(buf, 0, BUFFER_SIZE);

    get_log_prefix_GOOD(buf, BUFFER_SIZE);
    printf("Safe prefix generated: %s\n", buf);

    return APP_SUCCESS;
}