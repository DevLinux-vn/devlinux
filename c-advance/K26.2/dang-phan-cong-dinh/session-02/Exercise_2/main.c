/**
 * @file main.c
 * @brief Session 02 - Exercise 2: Stack Depth Monitor
 *
 * Ham de quy do luong stack da dung o moi tang, neu vuot qua gioi han
 * thi dung lai de tranh bi tran stack.
 *
 * Ghi chu MISRA 17.2: MISRA cam de quy vi khong biet truoc stack se
 * dung bao nhieu, de quy sau qua se tran stack va lam hong bo nho.
 * O bai nay em van dung de quy nhung co 2 dieu kien dung:
 *   - max_depth: gioi han so tang
 *   - stack_limit_bytes: gioi han so byte stack
 * nen stack khong the tang vo han.
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth,
                            const uintptr_t stack_base_addr,
                            uint32_t stack_limit_bytes);

/**
 * @brief Ham de quy theo doi stack, dung lai neu vuot gioi han.
 *
 * @param[in] current_depth     Tang hien tai (bat dau tu 0).
 * @param[in] max_depth         So tang toi da.
 * @param[in] stack_base_addr   Dia chi bien local trong main.
 * @param[in] stack_limit_bytes Gioi han stack (byte).
 * @return 0 neu chay het max_depth, -1 neu cham gioi han stack.
 */
int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth,
                            const uintptr_t stack_base_addr,
                            uint32_t stack_limit_bytes)
{
    uint8_t stack_marker = 0;   /* khoi tao truoc khi dung (MISRA 9.1) */
    uintptr_t stack_used = 0;

    /* stack moc xuong nen lay base tru dia chi hien tai */
    stack_used = stack_base_addr - (uintptr_t)&stack_marker;

    if (stack_used >= stack_limit_bytes)
    {
        printf("[Depth %2" PRIu32 "] WARNING: Stack usage (%" PRIuPTR
               " bytes) exceeds limit! Aborting recursion.\n",
               current_depth, stack_used);
        return -1;
    }

    printf("[Depth %2" PRIu32 "] stack_marker addr: %p, stack used: %4" PRIuPTR " bytes\n",
           current_depth, (void *)&stack_marker, stack_used);

    if (current_depth >= max_depth)
    {
        return 0;
    }

    return recurse_with_monitor(current_depth + 1, max_depth,
                                stack_base_addr, stack_limit_bytes);
}

/**
 * @brief Ham main.
 * @return 0 neu thanh cong, 1 neu cham gioi han stack.
 */
int main(void)
{
    uint8_t base_var = 0;
    uintptr_t stack_base_addr = (uintptr_t)&base_var;
    int8_t result = 0;

    printf("=== Stack Depth Monitor (limit: %d bytes) ===\n", 4096);

    result = recurse_with_monitor(0, 100, stack_base_addr, 4096);

    if (result == -1)
    {
        printf("Result: -1 (stack limit reached)\n");
        return 1;
    }

    printf("Result: 0 (max depth reached)\n");
    return 0;
}
