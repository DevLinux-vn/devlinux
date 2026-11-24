#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

/*
 * MISRA-C Rule 17.2:
 * Recursion is intentionally used for stack monitoring.
 * Recursion may cause unbounded stack growth.
 * The depth limit and stack usage threshold restrict
 * further recursive calls, but do not guarantee prevention
 * of a real stack overflow.
 */
/**
 * @brief Monitor estimated stack usage during recursion.
 *
 * @param current_depth Current recursion depth.
 * @param max_depth Maximum permitted recursion depth.
 * @param stack_base_addr Reference stack address.
 * @param stack_limit_bytes Stack usage threshold in bytes.
 *
 * @return 0 if maximum depth is reached without hitting
 *         the stack threshold.
 * @return -1 if the estimated stack threshold is reached.
 */
int8_t recurse_with_monitor(
    uint32_t current_depth,
    uint32_t max_depth,
    const uintptr_t stack_base_addr,
    uint32_t stack_limit_bytes)
{
    uint8_t stack_marker = 0U;

    uintptr_t current_addr = (uintptr_t)&stack_marker;
    uintptr_t stack_used;

    /* Calculate stack usage */
    if (stack_base_addr >= current_addr)
    {
        stack_used = stack_base_addr - current_addr;
    }
    else
    {
        stack_used = current_addr - stack_base_addr;
    }

    /* Print monitoring information */
    printf("Depth: %" PRIu32
           " | Address: %p"
           " | Stack used: %" PRIuPTR " bytes\n",
           current_depth,
           (void *)&stack_marker,
           stack_used);

    /* Check stack limit */
    if (stack_used >= (uintptr_t)stack_limit_bytes)
    {
        printf("WARNING: Stack limit reached!\n");
        return -1;
    }

    /* Check maximum recursion depth */
    if (current_depth >= max_depth)
    {
        return 0;
    }

    /* Continue recursion */
    return recurse_with_monitor(
        current_depth + 1U,
        max_depth,
        stack_base_addr,
        stack_limit_bytes);
}

int main(void)
{
    uint8_t stack_base_marker = 0U;

    uintptr_t stack_base_addr =
        (uintptr_t)&stack_base_marker;

    uint32_t max_depth = 10U;
    uint32_t stack_limit_bytes = 4096U;
    int8_t result = recurse_with_monitor(
        0U,
        max_depth,
        stack_base_addr,
        stack_limit_bytes);

    printf("Result: %" PRId8 "\n", result);

    if (result != 0)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
