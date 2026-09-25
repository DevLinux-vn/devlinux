#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const uint32_t global_const = 129U;  // Constant global variable (RODATA segment)
uint32_t global_init = 12U;          // Initialized global variable (DATA segment)
uint32_t global_uninit;              // Uninitialized global variable (BSS segment)

typedef union {
  void (*p_func)(void);
  void *p_void;
  uintptr_t addr;
} func_conv_t;

void print_memory_map(void){
    uint32_t* p_heap_var = (uint32_t*)malloc(sizeof(uint32_t)); // Dynamically allocated variable (HEAP segment)
    uint32_t stack_var = 24U; // Local variable (STACK segment)

    if (NULL == p_heap_var) {
        return;
    }

    func_conv_t text_conv;
    text_conv.p_func = &print_memory_map;

    (void)printf("=== Memory Segment Map ===\n");
    (void)printf("[TEXT]   Address of print_memory_map(): %p\n", text_conv.p_void);
    (void)printf("[RODATA] Address of global_const:       %p\n", (const void *)&global_const);
    (void)printf("[DATA]   Address of global_init:        %p\n", (void *)&global_init);
    (void)printf("[BSS]    Address of global_uninit:      %p\n", (void *)&global_uninit);
    (void)printf("[HEAP]   Address of heap_var:           %p\n", (void *)p_heap_var);
    (void)printf("[STACK]  Address of stack_var:          %p\n", (void *)&stack_var);

    uintptr_t addr_text   = text_conv.addr;
    uintptr_t addr_rodata = (uintptr_t)&global_const;
    uintptr_t addr_data   = (uintptr_t)&global_init;
    uintptr_t addr_bss    = (uintptr_t)&global_uninit;
    uintptr_t addr_heap   = (uintptr_t)p_heap_var;
    uintptr_t addr_stack  = (uintptr_t)&stack_var;

    (void)printf("\n=== Segment Distances ===\n");
    (void)printf("RODATA - TEXT:     %lu bytes\n", (unsigned long)(addr_rodata - addr_text));
    (void)printf("DATA   - RODATA:   %lu bytes\n", (unsigned long)(addr_data - addr_rodata));
    (void)printf("BSS    - DATA:     %lu bytes\n", (unsigned long)(addr_bss - addr_data));
    (void)printf("HEAP   - BSS:      %lu bytes\n", (unsigned long)(addr_heap - addr_bss));
    (void)printf("STACK  - HEAP:     %lu bytes\n", (unsigned long)(addr_stack - addr_heap));

    free(p_heap_var);
    p_heap_var = NULL;
}

int main(){
    print_memory_map();
    return 0;
}