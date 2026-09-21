#include <stdio.h>
#include <stdint.h>

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
                             uint32_t stack_limit_bytes)
{
    uint8_t stack_marker;
    uintptr_t addr_stack_marker = (uintptr_t)&stack_marker;
    uintptr_t stack_usage = stack_base_addr - addr_stack_marker;

    printf("current depth = %d\n", current_depth);
    printf(" address of stack marker = %p\n", (void *)addr_stack_marker);
    printf(" stack usage = %lld\n", (long long)stack_usage);
    if (stack_usage > stack_limit_bytes)
    {
        printf("stack over flow\n");
        return -1;
    }

    if(current_depth == max_depth) return 0;

    return recurse_with_monitor(current_depth +1, max_depth, stack_base_addr, stack_limit_bytes);
}

int main(){

    uint8_t local_variable;
    uintptr_t stack_base_addr = (uintptr_t)&local_variable;

    recurse_with_monitor(0, 100, stack_base_addr, 4096);

    return 0;


}
