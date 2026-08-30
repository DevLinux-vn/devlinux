#include "safe_string.h"
#include <string.h>
#include <stdio.h>

void safe_concat(char *p_dst, size_t dst_size, const char *p_src) {
    if (NULL == p_dst || NULL == p_src || dst_size == 0) {
        return;
    }

    size_t len = strlen(p_dst);
    if (len >= dst_size) {
        return;
    }

    strncat(p_dst, p_src, dst_size - len - 1);
}

void format_sensor_report(char *p_buf, size_t buf_size, const char *p_sensor_name, int32_t value, const char *p_unit) {
    if (NULL == p_buf || NULL == p_sensor_name || NULL == p_unit || buf_size == 0) {
        return;
    }

    snprintf(p_buf, buf_size, "[%s] VAL:%d %s", p_sensor_name, value, p_unit);
}

void get_log_prefix_GOOD(char *p_buf, size_t buf_size) {
    if (NULL == p_buf || buf_size == 0) {
        return;
    }

    snprintf(p_buf, buf_size, "%s", "[SYS_LOG]");
}