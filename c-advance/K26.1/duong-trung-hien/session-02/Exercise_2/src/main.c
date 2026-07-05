#include "stack_depth_monitor.h"

int main()
{
    uint8_t stack_marker = 1;
    uintptr_t stack_addr_marker = (uintptr_t)&stack_marker;

    printf("=== Stack Depth Monitor (limit: %u bytes) ===\n", STACK_LIMIT_BYTES);
    recurse_with_monitor(0, MAX_DEPTH, stack_addr_marker, STACK_LIMIT_BYTES);
    
    return 0;
}