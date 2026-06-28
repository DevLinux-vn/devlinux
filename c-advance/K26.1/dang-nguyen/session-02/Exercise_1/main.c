#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define APP_SUCCESS (0)
#define APP_FAILURE (-1)

#define ARG_NUM     (2)

int32_t (*p_main_func)(void);
const uint32_t global_const = 100U;
uint32_t global_init = 42;
uint32_t global_uninit;

void print_memory_map(void);

void print_memory_map(void)
{
    uint32_t *p_ptr = (uint32_t *)malloc(sizeof(uint32_t));
    uint32_t my_var = 1U;

    printf("=== Memory Segment Map ===\n");
    printf("[TEXT]   Address of main():            0x%lx\n", (uintptr_t)p_main_func);
    printf("[RODATA] Address of global_const:      0x%lx\n", (uintptr_t)&global_const);
    printf("[DATA]   Address of global_init:       0x%lx\n", (uintptr_t)&global_init);
    printf("[BSS]    Address of global_uninit:     0x%lx\n", (uintptr_t)&global_uninit);
    printf("[HEAP]   Address of heap_var:          0x%lx\n", (uintptr_t)p_ptr);
    printf("[STACK]  Address of stack_var:         0x%lx\n", (uintptr_t)&my_var);

    free(p_ptr);
    p_ptr = NULL;
}

int32_t main(void)
{
    p_main_func = &main;

    print_memory_map();

    return 0;
}