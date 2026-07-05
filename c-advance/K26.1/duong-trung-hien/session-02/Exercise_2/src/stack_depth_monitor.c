#include "stack_depth_monitor.h"

int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth,
                             const uintptr_t stack_base_addr,
                             uint32_t stack_limit_bytes)
{
    uint8_t stack_marker = 1;
    uintptr_t stack_addr_marker = (uintptr_t)&stack_marker;
    uint32_t stack_used;

    if (stack_base_addr > stack_addr_marker)
    {
        stack_used = stack_base_addr - stack_addr_marker;
    }
    else
    {
        stack_used = stack_addr_marker - stack_base_addr;
    }

    if (stack_used > stack_limit_bytes)
    {
        printf("[Depth %u] WARNING: Stack usage (%u bytes) exceeds limit! Aborting recursion.\n", current_depth, stack_used);
        return -1;
    }

    printf("[Depth  %u] stack_marker addr: %p, stack used:    %u bytes\n", current_depth, (void *)&stack_marker, stack_used);

    if(current_depth >= max_depth)
    {
        return 0;
    }

    return recurse_with_monitor(current_depth + 1, 
                                max_depth,
                                stack_base_addr,
                                stack_limit_bytes);
}
