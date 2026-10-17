#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth, const uintptr_t stack_base_addr, uint32_t stack_limit_bytes)
{
    uint8_t stack_marker = 0U;

    uintptr_t current_stack_addr = (uintptr_t)&stack_marker;

    uint32_t stack_used = (uint32_t)(stack_base_addr - current_stack_addr);

    printf("[Depth %2u] stack_marker addr: %p, stack used: %4u bytes\n", current_depth, (void *)&stack_marker, stack_used);

    if (stack_used >= stack_limit_bytes)
    {
    printf("[Depth %2u] WARNING: Stack usage (%u bytes) exceeds limit! Aborting recursion.\n",
           current_depth,
           stack_used);

    return -1;
    }

    if (current_depth >= max_depth)
    {
    return 0;
    }

    return recurse_with_monitor(current_depth + 1U, max_depth, stack_base_addr, stack_limit_bytes);
}
