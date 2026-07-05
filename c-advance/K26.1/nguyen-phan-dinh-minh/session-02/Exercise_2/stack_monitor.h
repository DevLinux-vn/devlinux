#ifndef STACK_MONITOR_H
#define STACK_MONITOR_H
#include <stdint.h>

int8_t recurse_with_monitor(uint32_t current_depth,  uint32_t max_depth, const uintptr_t stack_base_addr, uint32_t stack_limit_bytes);

#endif
