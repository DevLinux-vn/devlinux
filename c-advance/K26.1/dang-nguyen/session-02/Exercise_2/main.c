#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#define EC_SUCCESS          ((int8_t)0)
#define EC_FAILURE          ((int8_t)(-1))

#define MAX_DEPTH           (100U)
#define STACK_LIMIT_BYTES   (4096U)

/**
 * @brief Monitor stack depth during recursion and abort if limit is exceeded.
 *
 * @param[in] current_depth     Current recursion depth (starts at 0).
 * @param[in] max_depth         Maximum recursion depth to attempt.
 * @param[in] stack_base_addr   Address of a local variable in main(), cast to uintptr_t.
 * @param[in] stack_limit_bytes Maximum allowed stack consumption in bytes.
 * @return 0 on success (max_depth reached), -1 if stack limit was exceeded.
 */
int8_t recurse_with_monitor(
    uint32_t        current_depth,
    uint32_t        max_depth,
    const uintptr_t stack_base_addr,
    uint32_t        stack_limit_bytes
);

int8_t recurse_with_monitor(
    uint32_t        current_depth,
    uint32_t        max_depth,
    const uintptr_t stack_base_addr,
    uint32_t        stack_limit_bytes
)
{
    int8_t ret = EC_SUCCESS;
    
    uint8_t stack_marker = 0U;
    uintptr_t stack_marker_addr = (uintptr_t)&stack_marker;
    uintptr_t stack_used = (stack_base_addr > stack_marker_addr) ?
                            (stack_base_addr - stack_marker_addr) :
                            (stack_marker_addr - stack_base_addr);

    printf("[Depth %u] stack_marker addr: 0x%" PRIxPTR ", stack used: %" PRIuPTR " bytes\n", current_depth, stack_marker_addr, stack_used);

    if (stack_used >= stack_limit_bytes)
    {
        printf("WARNING: Stack usage (%" PRIuPTR " bytes) exceeds limit! Aborting recursion\n", stack_used);
        ret = EC_FAILURE;
        printf("Result: %" PRId32 " (stack limit reached at depth %" PRIu32 ")\n", ret, current_depth);
    }
    else if (current_depth >= max_depth)
    {
        ret = EC_SUCCESS;
    }
    else
    {
        /*
         * MISRA-C 2012 Rule 17.2 forbids recursion because recursive calls can grow
         * the stack unboundedly, risking stack overflow and memory corruption.
         * This implementation is safe because:
         * 1. stack_limit_bytes guard prevents recursion before overflow occurs
         * 2. Each level compares current stack_used against limit BEFORE recursing
         * 3. Function returns immediately upon limit detection
         * This makes recursion usage controlled and predictable.
         */
        ret = recurse_with_monitor(current_depth + 1U, max_depth, stack_base_addr, stack_limit_bytes);
    }

    return ret;
}

/**
 * @brief Entry point for stack depth monitor demonstration.
 * 
 * Initializes stack base address and calls recurse_with_monitor() to demonstrate
 * stack consumption tracking and overflow detection.
 * 
 * @return 0 on success, -1 if stack limit was exceeded.
 */
int32_t main(void)
{
    int32_t ret = EC_SUCCESS;
    int32_t stack_base_marker = 0;
    uintptr_t base_addr = (uintptr_t)&stack_base_marker;
    uint32_t cur_depth = 0U;

    ret = (int32_t)recurse_with_monitor(cur_depth, MAX_DEPTH, base_addr, STACK_LIMIT_BYTES);

    if (EC_SUCCESS == ret)
    {
        printf("Success!\n");
    }
    else
    {
        printf("Failure!\n");
    }

    return ret;
}