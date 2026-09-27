#include <stdio.h>
#include <stdint.h>
#include "stack_monitor.h"

int main(void)
{
    uint8_t stack_base = 0U;

    uintptr_t stack_base_addr = (uintptr_t)&stack_base;

    int8_t result = recurse_with_monitor(0U, 100U, stack_base_addr, 4096U);

    printf("\nResult: %d\n", result);

    return 0;
}
