/**
 * @file safe_string.h
 * @brief Function prototypes for safe string manipulation.
 */
#ifndef SAFE_STRING_H
#define SAFE_STRING_H

#include <stdint.h>
#include <stddef.h>

/** 
 * @brief Safely concatenate p_src onto p_dst without overflowing dst_size. 
 *
 * @param[in,out] p_dst     Destination buffer.
 * @param[in]     dst_size  Total size of the destination buffer.
 * @param[in]     p_src     Source string to append.
 */
void safe_concat(char *p_dst, size_t dst_size, const char *p_src);

/** 
 * @brief Format a sensor report into p_buf using snprintf. 
 *
 * @param[out] p_buf         Destination buffer for the report.
 * @param[in]  buf_size      Size of the destination buffer.
 * @param[in]  p_sensor_name Name of the sensor.
 * @param[in]  value         Sensor reading value.
 * @param[in]  p_unit        Unit of measurement.
 */
void format_sensor_report(char *p_buf, size_t buf_size, 
                          const char *p_sensor_name, int32_t value, 
                          const char *p_unit);

/** 
 * @brief Returns a dangling pointer to a local array. (DANGEROUS)
 * 
 * @return A pointer to a stack-allocated string.
 */
char* get_log_prefix_BAD(void);

/** 
 * @brief Safe log prefix using caller-provides-buffer pattern. 
 *
 * @param[out] p_buf    Buffer provided by the caller.
 * @param[in]  buf_size Size of the provided buffer.
 */
void get_log_prefix_GOOD(char *p_buf, size_t buf_size);

#endif /* SAFE_STRING_H */