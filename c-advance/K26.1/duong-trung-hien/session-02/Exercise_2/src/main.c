#include "stack_depth_monitor.h"

int main()
{
    uint8_t stack_marker = 1;
    uintptr_t stack_addr_marker = (uintptr_t)&stack_marker;

    printf("=== Stack Depth Monitor (limit: %u bytes) ===\n", STACK_LIMIT_BYTES);

    int8_t result = recurse_with_monitor(0, MAX_DEPTH, stack_addr_marker, STACK_LIMIT_BYTES);
    if (result == 0) 
    {
        printf("Result: 0 (max depth reached)\n");
    } else {
        printf("Result: -1 (stack limit reached)\n");
    }

    return 0;
}