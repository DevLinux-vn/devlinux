#ifndef SAFE_STRING_H
#define SAFE_STRING_H

#include <stdint.h>
#include <stddef.h>

#define BUFFER_SIZE         (128)
#define TINY_BUFFER_SIZE    (10)

/**
 * @brief Safely concatenates p_src onto p_dst without overflowing dst_size.
 *
 * @param[in,out] p_dst Destination buffer.
 * @param[in]     dst_size Size of the destination buffer.
 * @param[in]     p_src Source string to append.
 */
void safe_concat(char *p_dst, size_t dst_size, const char *p_src);

/**
 * @brief Safely formats a sensor report into p_buf using snprintf.
 *
 * @param[out] p_buf Destination buffer for the formatted report.
 * @param[in]  buf_size Size of the destination buffer.
 * @param[in]  p_sensor_name Sensor name.
 * @param[in]  value Sensor value.
 * @param[in]  p_unit Sensor value unit.
 */
void format_sensor_report(char *p_buf, size_t buf_size,
                          const char *p_sensor_name, int32_t value,
                          const char *p_unit);

/**
 * @brief Safely writes a log prefix into a caller-provided buffer.
 *
 * @param[out] p_buf Destination buffer for the log prefix.
 * @param[in]  buf_size Size of the destination buffer.
 */
void get_log_prefix_GOOD(char *p_buf, size_t buf_size);

#endif /* SAFE_STRING_H */