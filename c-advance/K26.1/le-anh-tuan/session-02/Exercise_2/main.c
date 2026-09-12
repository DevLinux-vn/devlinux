#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#define STACK_MONITOR_MAX_DEPTH       (100U)
#define STACK_MONITOR_LIMIT_BYTES     (4096U)
#define STACK_MONITOR_SUCCESS         (0)
#define STACK_MONITOR_ERROR           (-1)
#define STACK_MONITOR_DEPTH_STEP      (1U)

/**
 * @brief Calculate absolute distance between two addresses.
 *
 * @param[in] first_addr  First address value.
 * @param[in] second_addr Second address value.
 * @return Absolute distance in bytes.
 */
static uintptr_t calculate_address_distance(uintptr_t first_addr,
                                            uintptr_t second_addr)
{
    uintptr_t distance_bytes = 0U;

    if (first_addr >= second_addr)
    {
        distance_bytes = first_addr - second_addr;
    }
    else
    {
        distance_bytes = second_addr - first_addr;
    }

    return distance_bytes;
}

/**
 * @brief Monitor stack depth during recursion and abort if limit is exceeded.
 *
 * @param[in] current_depth     Current recursion depth.
 * @param[in] max_depth         Maximum recursion depth to attempt.
 * @param[in] stack_base_addr   Address of a local variable in main().
 * @param[in] stack_limit_bytes Maximum allowed stack consumption in bytes.
 * @return 0 on success, -1 if the stack limit was exceeded.
 *
 * @note MISRA-C 2012 Rule 17.2 forbids recursion because recursive calls may
 *       cause unbounded stack growth. This exercise intentionally uses
 *       recursion for stack-monitoring practice, and stack_limit_bytes is used
 *       as a guard to stop before real stack overflow occurs.
 */
static int8_t recurse_with_monitor(uint32_t current_depth,
                                   uint32_t max_depth,
                                   uintptr_t stack_base_addr,
                                   uint32_t stack_limit_bytes)
{
    uint8_t stack_marker = 0U;
    uintptr_t current_stack_addr = (uintptr_t)&stack_marker;
    uintptr_t stack_used_bytes = calculate_address_distance(stack_base_addr,
                                                            current_stack_addr);
    int8_t result = STACK_MONITOR_SUCCESS;

    printf("[Depth %3" PRIu32 "] stack_marker addr: 0x%" PRIxPTR
           ", stack used: %4" PRIuPTR " bytes\n",
           current_depth,
           current_stack_addr,
           stack_used_bytes);

    if (stack_used_bytes >= (uintptr_t)stack_limit_bytes)
    {
        printf("[Depth %3" PRIu32 "] WARNING: Stack usage (%" PRIuPTR
               " bytes) exceeds limit! Aborting recursion.\n",
               current_depth,
               stack_used_bytes);

        result = STACK_MONITOR_ERROR;
    }
    else if (current_depth >= max_depth)
    {
        result = STACK_MONITOR_SUCCESS;
    }
    else
    {
        result = recurse_with_monitor(current_depth + STACK_MONITOR_DEPTH_STEP,
                                      max_depth,
                                      stack_base_addr,
                                      stack_limit_bytes);
    }

    return result;
}

/**
 * @brief Program entry point.
 *
 * @return 0 on success, non-zero on error.
 */
int main(void)
{
    uint8_t stack_base_marker = 0U;
    uintptr_t stack_base_addr = (uintptr_t)&stack_base_marker;
    int8_t result = STACK_MONITOR_SUCCESS;
    int exit_code = 0;

    printf("=== Stack Depth Monitor (limit: %" PRIu32 " bytes) ===\n",
           STACK_MONITOR_LIMIT_BYTES);

    result = recurse_with_monitor(0U,
                                  STACK_MONITOR_MAX_DEPTH,
                                  stack_base_addr,
                                  STACK_MONITOR_LIMIT_BYTES);

    if (result == STACK_MONITOR_SUCCESS)
    {
        printf("Result: %" PRId8 " maximum depth reached safely\n", result);
        exit_code = 0;
    }
    else
    {
        printf("Result: %" PRId8 " stack limit reached\n", result);
        exit_code = 1;
    }

    return exit_code;
}