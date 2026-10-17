
#include <stdint.h>
#include <stdio.h>
static int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth,const uintptr_t stack_base_addr,uint32_t stack_limit_bytes);
int main(void){
    uint8_t stack_base=0U;
    int8_t result = 0;
    uintptr_t base_addr = (uintptr_t)&stack_base;
    printf("=== Stack Depth Monitor (limit: 4096 bytes) ===\n");
    result = recurse_with_monitor(0, 100, base_addr, 4096);
    if (result == 0)
    {
        printf("Result: %d (OK)\n", result);
    }
    else
    {
        printf("Result: %d (stack limit reached)\n", result);
    }
    return 0;
}
/**
 * @brief Monitor stack depth during recursion and abort if limit is exceeded.
 *
 * @param[in] current_depth     Current recursion depth (starts at 0).
 * @param[in] max_depth         Maximum recursion depth to attempt.
 * @param[in] stack_base_addr   Address of a local variable in main(), cast to uintptr_t.
 * @param[in] stack_limit_bytes Maximum allowed stack consumption in bytes.
 * @return 0 on success (max_depth reached), -1 if stack limit was exceeded.
 */
static int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth,const uintptr_t stack_base_addr,uint32_t stack_limit_bytes){
    uint8_t stack_marker=0U;
    uint32_t stack_used = stack_base_addr - (uintptr_t)&stack_marker;
    printf("[Depth %2u] stack_marker addr: %p, Stack used: %5u bytes\n", current_depth, (void*)&stack_marker, stack_used);
    if (stack_used >= stack_limit_bytes)
    {
        printf("[Depth %2u] WARNING: Stack usage (%5u bytes) exceeds limit! Aborting recursion.\n",current_depth, stack_used);
        return -1;
    }
    if (current_depth >= max_depth)
    {
        return 0;
    }
    return recurse_with_monitor(current_depth + 1, max_depth, stack_base_addr, stack_limit_bytes);
}