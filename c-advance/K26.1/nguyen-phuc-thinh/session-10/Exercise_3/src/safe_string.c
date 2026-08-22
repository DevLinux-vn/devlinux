/**
 * @file safe_string.c
 * @brief Implementation of safe string operations.
 */
#include "safe_string.h"
#include <stdio.h>
#include <string.h>

void safe_concat(char *p_dst, size_t dst_size, const char *p_src)
{
    if ((p_dst == NULL) || (p_src == NULL) || (dst_size == 0U))
    {
        return;
    }

    size_t current_len = strlen(p_dst);
    
    /* Ensure there is room to concatenate */
    if (current_len >= dst_size - 1U)
    {
        return; 
    }

    size_t remaining_space = dst_size - current_len;
    
    /* snprintf safely appends and ensures null-termination */
    snprintf(p_dst + current_len, remaining_space, "%s", p_src);
}

void format_sensor_report(char *p_buf, size_t buf_size, 
                          const char *p_sensor_name, int32_t value, 
                          const char *p_unit)
{
    if ((p_buf == NULL) || (buf_size == 0U) || 
        (p_sensor_name == NULL) || (p_unit == NULL))
    {
        return;
    }
    
    snprintf(p_buf, buf_size, "[%s] VAL:%d %s", p_sensor_name, value, p_unit);
}

/* 
 * Bỏ qua cảnh báo "return-local-addr" của GCC để trình biên dịch không 
 * đánh rớt dự án (do cờ -Werror), phục vụ riêng cho mục đích demo lỗi.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
char* get_log_prefix_BAD(void)
{
    char prefix[] = "[SYS_LOG]";
    return prefix; /* DANGER: Returns pointer to destroyed stack frame (DCL30-C) */
}
#pragma GCC diagnostic pop

void get_log_prefix_GOOD(char *p_buf, size_t buf_size)
{
    if ((p_buf == NULL) || (buf_size == 0U))
    {
        return;
    }
    
    snprintf(p_buf, buf_size, "[SYS_LOG]");
}