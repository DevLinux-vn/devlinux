#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

/**
 * @file main.c
 * @brief Stack depth monitoring example using bounded recursion.
 */

/* =========================
 * Defines
 * ========================= */

/** @brief Application success return value. */
#define APP_SUCCESS                 (0)

/** @brief Application failure return value. */
#define APP_FAILURE                 (-1)

/** @brief Default maximum recursion depth. */
#define DEFAULT_MAX_DEPTH           (100U)

/** @brief Default stack usage limit in bytes. */
#define DEFAULT_STACK_LIMIT_BYTES   (4096U)

/**
 * @brief Monitor stack usage during recursive calls.
 *
 * This function recursively calls itself until either:
 * - the maximum recursion depth is reached, or
 * - the estimated stack usage exceeds the configured limit.
 *
 * @note Recursion is normally discouraged in safety-critical C coding
 *       standards such as MISRA C. In this example, recursion is bounded
 *       by @p max_depth and protected by @p stack_limit_bytes.
 *
 * @param[in] current_depth      Current recursion depth.
 * @param[in] max_depth          Maximum allowed recursion depth.
 * @param[in] stack_base_addr    Stack base address captured before recursion.
 * @param[in] stack_limit_bytes  Maximum allowed stack usage in bytes.
 *
 * @return APP_SUCCESS if recursion finishes safely.
 * @return APP_FAILURE if stack usage exceeds the configured limit.
 */
int8_t recurse_with_monitor(uint32_t current_depth,
                            uint32_t max_depth,
                            const uintptr_t stack_base_addr,
                            uint32_t stack_limit_bytes)
{
    int8_t ret = APP_SUCCESS;

    /** Local variable used as a stack address marker. */
    uint8_t stack_marker = 0U;

    uintptr_t current_addr = (uintptr_t)&stack_marker;
    uint32_t stack_used = 0U;

    if (stack_base_addr > current_addr)
    {
        stack_used = (uint32_t)(stack_base_addr - current_addr);
    }
    else
    {
        stack_used = (uint32_t)(current_addr - stack_base_addr);
    }

    (void)printf("[Depth %3" PRIu32 "] stack_marker addr: %p, stack used: %5" PRIu32 " bytes\n",
                 current_depth,
                 (void *)current_addr,
                 stack_used);

    if (stack_used >= stack_limit_bytes)
    {
        (void)printf("[Depth %3" PRIu32 "] WARNING: Stack usage (%" PRIu32
                     " bytes) exceeds limit! Aborting recursion.\n",
                     current_depth,
                     stack_used);
        ret = APP_FAILURE;
    }
    else if (current_depth >= max_depth)
    {
        ret = APP_SUCCESS;
    }
    else
    {
        ret = recurse_with_monitor(current_depth + 1U,
                                   max_depth,
                                   stack_base_addr,
                                   stack_limit_bytes);
    }

    return ret;
}

/**
 * @brief Program entry point.
 *
 * Initializes stack monitoring parameters, captures the initial stack
 * address, and starts bounded recursive stack monitoring.
 *
 * @return 0 if stack monitoring completes successfully.
 * @return 1 if stack usage exceeds the configured limit.
 */
int main(void)
{
    int8_t result = APP_SUCCESS;

    /** Variable used to capture the initial stack address. */
    uint8_t base_stack_var = 0U;

    uintptr_t stack_base_addr = (uintptr_t)&base_stack_var;

    uint32_t max_depth = DEFAULT_MAX_DEPTH;
    uint32_t stack_limit_bytes = DEFAULT_STACK_LIMIT_BYTES;

    (void)printf("=== Stack Depth Monitor (limit: %" PRIu32 " bytes) ===\n",
                 stack_limit_bytes);

    result = recurse_with_monitor(0U,
                                  max_depth,
                                  stack_base_addr,
                                  stack_limit_bytes);

    (void)printf("Result: %" PRId8 "\n", result);

    return (result == APP_SUCCESS) ? 0 : 1;
}