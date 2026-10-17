/**
 * @file    safe_string.h
 * @brief   Bounded, buffer-safe string helper functions, and a
 *          deliberate counter-example of the dangling pointer hazard.
 */
#ifndef SAFE_STRING_H
#define SAFE_STRING_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief   Safely concatenate @p p_src onto the end of @p p_dst.
 *
 * @details Never writes past @p dst_size bytes (including the null
 *          terminator). If @p p_src does not fully fit, the result is
 *          safely truncated rather than overflowing the buffer.
 *
 * @param[in,out] p_dst    Destination buffer, already null-terminated.
 *                          May be NULL only if @p dst_size is 0.
 * @param[in]     dst_size Total size of @p p_dst, in bytes.
 * @param[in]     p_src    String to append. May be NULL (no-op).
 */
void safe_concat(char *p_dst, size_t dst_size, const char *p_src);

/**
 * @brief   Format a sensor report string, bounded to @p buf_size.
 *
 * @param[out] p_buf         Destination buffer.
 * @param[in]  buf_size      Total size of @p p_buf, in bytes.
 * @param[in]  p_sensor_name Sensor name (e.g. "PRESSURE").
 * @param[in]  value         Sensor reading value.
 * @param[in]  p_unit        Unit string (e.g. "hPa").
 */
void format_sensor_report(char *p_buf, size_t buf_size,
                           const char *p_sensor_name, int32_t value,
                           const char *p_unit);

/**
 * @brief   UNSAFE EXAMPLE: returns a pointer to a local stack array.
 *
 * @details This function is intentionally broken to demonstrate the
 *          dangling pointer hazard (CERT DCL30-C). Its return value
 *          becomes invalid the instant the function returns, because
 *          the array it points to lives on the stack frame that is
 *          torn down at that point. DO NOT call this function -
 *          see main.c for the commented-out example and explanation.
 *
 * @return  A dangling pointer to a stack array that no longer exists.
 */
char *get_log_prefix_BAD(void);

/**
 * @brief   Safe alternative to get_log_prefix_BAD(), using the
 *          caller-provides-buffer pattern.
 *
 * @param[out] p_buf    Caller-owned buffer to receive the prefix.
 * @param[in]  buf_size Total size of @p p_buf, in bytes.
 */
void get_log_prefix_GOOD(char *p_buf, size_t buf_size);

#endif /* SAFE_STRING_H */