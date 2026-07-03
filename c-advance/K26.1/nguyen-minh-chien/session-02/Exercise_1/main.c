/**
 * @brief   Print the address of each memory segment
 *
 * @details Declares variables in each memory segment (text, rodata,
 *          data, bss, heap, stack), prints their addresses, and
 *          calculates the distance between segments using uintptr_t
 *          arithmetic. Heap memory is allocated with malloc() and
 *          freed before the function returns.
 *
 * @return  void
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
// declare variables
const uint32_t global_const = 100;
uint32_t global_init = 42;
uint32_t global_uninit;
static void print_memory_map(void);
int main(void) {
    // Your code here
    print_memory_map();
    return 0;
}
static void print_memory_map(void){
    // Your code here
    /* khai báo p_text là con trỏ hàm, trỏ đến main */
    int local_var = 10;
    union {
    int (*fn)(void);    /* khớp với int main(void) */
    void *p_obj;
    } u_func;
    u_func.fn = main;
    printf("=== Memory Segment Map ===\n"); 
    printf("[TEXT]   Address of main(): %p\n", u_func.p_obj);
    printf("[RODATA] Address of global_const: %p\n", (void*)&global_const);
    printf("[DATA]   Address of global_init: %p\n", (void*)&global_init);
    printf("[BSS]    Address of global_uninit: %p\n", (void*)&global_uninit);
    printf("[STACK]  Address of local_var: %p\n", (void*)&local_var);
    /* HEAP    */
    uint32_t *p_heap = (uint32_t *)malloc(sizeof(uint32_t));
    if (p_heap == NULL) {
        printf("Memory allocation failed\n");
        return; // Handle malloc failure
    }
    uintptr_t addr_heap = (uintptr_t)p_heap; // save address of heap memory
    printf("[HEAP]   Address of HEAP memory: %p\n", (void*)p_heap);
    free(p_heap); // Free the allocated memory
    p_heap = NULL; // Set the pointer to NULL after freeing
    /*SEGMENT DISTANCE*/
    printf("\n=== Memory Segment Distance ===\n");
    uintptr_t addr_text   = (uintptr_t)u_func.p_obj;
    uintptr_t addr_rodata = (uintptr_t)&global_const;
    uintptr_t addr_data   = (uintptr_t)&global_init;
    uintptr_t addr_bss    = (uintptr_t)&global_uninit;
    uintptr_t addr_stack  = (uintptr_t)&local_var;
    printf("RODATA - TEXT: %llu bytes\n", addr_rodata - addr_text);
    printf("DATA - RODATA: %llu bytes\n", addr_data - addr_rodata);
    printf("BSS - DATA: %llu bytes\n", addr_bss - addr_data);
    printf("HEAP - BSS: %llu bytes\n", addr_heap - addr_bss);
    printf("STACK - HEAP: %llu bytes\n", addr_stack - addr_heap);

}
/*
 * Verification:
 *
 * $ size memory_analyzer.exe
 *    text    data     bss     dec     hex filename
 *   14200     192     384   14776    39b8 memory_analyzer.exe
 *
 * $ nm memory_analyzer.exe | grep -i global_init
 * 0000000140003000 D global_init
 *
 * $ nm memory_analyzer.exe | grep -i global_uninit
 * 0000000140007030 B global_uninit
 */