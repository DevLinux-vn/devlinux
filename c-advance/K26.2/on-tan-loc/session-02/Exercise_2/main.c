#include <stdint.h>
#include <stdio.h>

int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth,
                             const uintptr_t stack_base_addr,
                             uint32_t stack_limit) {
    int8_t ret_val = 0;
    uint32_t stack_used = 0;

    /*stack current address*/
    uint8_t stack_marker = 0;
    const uintptr_t p_stack_current = (uintptr_t)&stack_marker;
    
    if (stack_base_addr >= p_stack_current) {
        stack_used = (uint32_t)(stack_base_addr - p_stack_current);
    }

    printf("[Depth %u] stack_marker addr: 0x%lx, stack used: %u bytes\n", (unsigned int)current_depth,
                                                (unsigned long)p_stack_current, (unsigned int)stack_used);

    if (stack_used >= stack_limit) {
        printf("[Depth %u] WARNING: Stack usage (%u bytes) exceeds limit! Aborting recursion.\n",
                    (unsigned int)current_depth, (unsigned int)stack_used);
        ret_val = -1;
    } else if (current_depth >= max_depth) {
        ret_val = 0;
    } else {
        ret_val = recurse_with_monitor(current_depth + 1, max_depth,
                                       stack_base_addr, stack_limit);
    }

    return ret_val;
}

int main(void) {
    /*landmark to measure - the first address created in stack*/
    uint8_t stack_base = 0;
    uintptr_t stack_base_addr = (uintptr_t)&stack_base;

    const uint32_t start_measure = 0;
    const uint32_t max_depth = 100;
    const uint32_t stack_limit = 4096;

    int8_t ret = recurse_with_monitor(start_measure, max_depth, stack_base_addr, stack_limit);

    printf("=== Stack Depth Monitor (limit: %u bytes) ===\n", (unsigned int)stack_limit);
    printf("Result: %d (%s)\n", ret, (ret == 0) ? "max depth reached" : "stack limit reached");

    return 0;
}