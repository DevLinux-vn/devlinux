/**
 * @file    safe_string.c
 * @brief   Implementation of bounded string helpers, and the
 *          intentionally unsafe get_log_prefix_BAD() counter-example.
 */
#include "safe_string.h"
#include <stdio.h>
#include <string.h>

void safe_concat(char *p_dst, size_t dst_size, const char *p_src)
{
    /* CERT STR31-C: never write past dst_size, and only proceed with
     * valid pointers. */
    if ((p_dst != NULL) && (p_src != NULL) && (dst_size > 0U))
    {
        size_t cur_len = strlen(p_dst);

        if (cur_len < dst_size)
        {
            /* snprintf() never writes more than (dst_size - cur_len)
             * bytes total (including the null terminator), so this
             * can never overflow p_dst - it truncates safely instead. */
            (void)snprintf(p_dst + cur_len, dst_size - cur_len, "%s", p_src);
        }
    }
}

void format_sensor_report(char *p_buf, size_t buf_size,
                           const char *p_sensor_name, int32_t value,
                           const char *p_unit)
{
    if ((p_buf != NULL) && (p_sensor_name != NULL) && (p_unit != NULL) && (buf_size > 0U))
    {
        (void)snprintf(p_buf, buf_size, "[%s] VAL:%d %s", p_sensor_name, value, p_unit);
    }
}

/* Silence the compiler/analyzer warnings for this one function only:
 * returning the address of a local array is exactly the bug this
 * function exists to demonstrate (CERT DCL30-C). It is never called
 * anywhere in this project - see main.c for the commented-out call
 * and explanation. */
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
#endif

char *get_log_prefix_BAD(void)
{
    char local_prefix[16] = "[SYS_LOG] ";

    /* BUG (intentional): local_prefix lives on this function's stack
     * frame. Once we return, that memory is no longer ours - any
     * later function call is free to overwrite it. The caller ends
     * up with a dangling pointer. */
    // cppcheck-suppress returnDanglingLifetime
    return local_prefix; // NOLINT(clang-analyzer-core.StackAddressEscape)
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

void get_log_prefix_GOOD(char *p_buf, size_t buf_size)
{
    if ((p_buf != NULL) && (buf_size > 0U))
    {
        (void)snprintf(p_buf, buf_size, "[SYS_LOG] ");
    }
}