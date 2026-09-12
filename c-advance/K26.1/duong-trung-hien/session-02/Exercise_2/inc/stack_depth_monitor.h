#ifndef STACK_DEPTH_MONITOR_H
#define STACK_DEPTH_MONITOR_H

#include <stdint.h>
#include <stdio.h>

/* Configuration macros */
#define MAX_DEPTH            (100U)
#define STACK_LIMIT_BYTES     (4096U)

/**
 * @brief Monitor stack depth during recursion and abort if limit is exceeded.
 *
 * @param[in] current_depth     Current recursion depth (starts at 0).
 * @param[in] max_depth         Maximum recursion depth to attempt.
 * @param[in] stack_base_addr   Address of a local variable in main(), cast to uintptr_t.
 * @param[in] stack_limit_bytes Maximum allowed stack consumption in bytes.
 * @return 0 on success (max_depth reached), -1 if stack limit was exceeded.
 */
int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth,
                             const uintptr_t stack_base_addr,
                             uint32_t stack_limit_bytes);

#endif
