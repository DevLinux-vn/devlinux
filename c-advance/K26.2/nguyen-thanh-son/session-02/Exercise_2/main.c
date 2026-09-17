#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Recurse while tracking stack consumption, aborting safely if a
 *        configured byte limit is exceeded.
 * @param[in] current_depth     Current recursion depth (starts at 0).
 * @param[in] max_depth         Maximum recursion depth to attempt.
 * @param[in] stack_base_addr   Address of a local variable in main(), cast to uintptr_t.
 * @param[in] stack_limit_bytes Maximum allowed stack consumption in bytes.
 * @return 0 on success (max_depth reached), -1 if stack limit was exceeded.
 */
int8_t recurse_with_monitor(uint32_t current_depth,
                             uint32_t max_depth,
                             const uintptr_t stack_base_addr,
                             uint32_t stack_limit_bytes);

int main(void)
{
    uint8_t base_marker = 0U;
    const uintptr_t stack_base_addr = (uintptr_t)&base_marker;

    printf("=== Stack Depth Monitor (limit: 4096 bytes) ===\n");
    int8_t result = recurse_with_monitor(0U, 100U, stack_base_addr, 4096U);
    printf("Result: %" PRId8 " (%s)\n", result, (result == 0) ? "max depth reached" : "stack limit reached");

    return (result == 0) ? 0 : 1;
}

int8_t recurse_with_monitor(uint32_t current_depth,
                             uint32_t max_depth,
                             const uintptr_t stack_base_addr,
                             uint32_t stack_limit_bytes)
{
    uint8_t stack_marker = 0U;
    uintptr_t addr_marker = (uintptr_t)&stack_marker;
    uintptr_t stack_used = stack_base_addr - addr_marker;

    printf("[Depth %2" PRIu32 "] stack_marker addr: %p, stack used: %4" PRIuPTR " bytes\n",
           current_depth, (void *)addr_marker, stack_used);

    if (stack_used >= (uintptr_t)stack_limit_bytes)
    {
        printf("[Depth %2" PRIu32 "] WARNING: Stack usage (%" PRIuPTR " bytes) exceeds limit! Aborting recursion.\n",
               current_depth, stack_used);
        return -1;
    }
    if (current_depth >= max_depth)
    {
        return 0;
    }

    return recurse_with_monitor(current_depth + 1U, max_depth,
                                 stack_base_addr, stack_limit_bytes);
}
