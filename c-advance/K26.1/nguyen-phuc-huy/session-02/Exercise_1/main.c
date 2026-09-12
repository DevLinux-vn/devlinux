/**
 * @file main.c
 * @brief Memory Segment Analyzer — Map, Measure, and Verify
 *
 * Final stable version - crash fixed + clang-tidy friendly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Named constants */
#define GLOBAL_INIT_VALUE         42U
#define CUSTOM_VAR_VALUE          16
#define CUSTOM_ARRAY_SIZE         6U
#define HEAP_BUFFER_SIZE          64U
#define META_DATA_SECTION_NAME    "my_meta_data"

/* Array values */
#define ARRAY_VAL_0  30
#define ARRAY_VAL_1  20
#define ARRAY_VAL_2  68
#define ARRAY_VAL_3  64
#define ARRAY_VAL_4  62
#define ARRAY_VAL_5  58

/* Global variables */
uint32_t global_uninit;
uint32_t global_init_zero = 0U;
uint32_t global_init = GLOBAL_INIT_VALUE;
const uint32_t global_const = 100U;

/* Custom section */
__attribute__((section(META_DATA_SECTION_NAME)))
int32_t custom_var = CUSTOM_VAR_VALUE;

__attribute__((section(META_DATA_SECTION_NAME)))
int32_t custom_array[CUSTOM_ARRAY_SIZE] = {
    ARRAY_VAL_0, ARRAY_VAL_1, ARRAY_VAL_2,
    ARRAY_VAL_3, ARRAY_VAL_4, ARRAY_VAL_5
};

__attribute__((section(META_DATA_SECTION_NAME)))
const char *p_text_meta_data = "Hello devlinux this is my meta data";

/* Linker symbols */
extern const char __start_my_meta_data;
extern const char __stop_my_meta_data;

typedef void *(*p_address_getter_template)(void);

/* Prototypes */
void print_segment(const char *p_tag, const char *p_des, void *p_addr);
void *get_text_address(void);
void *get_rodata_address(void);
void alloc_data_heap_segment(char **p_buffer, uint32_t size);
void print_memory_map(void);

/**
 * @brief Simple address printer (no risky function pointer cast).
 *
 * @param p_tag Tag name
 * @param p_des Description
 * @param p_addr Address to print
 */
void print_segment(const char *p_tag, const char *p_des, void *p_addr)
{
    printf("%-10s %-30s %p\n", p_tag, p_des, p_addr);
}

/**
 * @brief Getter for .text
 */
void *get_text_address(void)
{
    return (void *)(uintptr_t)print_memory_map;
}

/**
 * @brief Getter for .rodata
 */
void *get_rodata_address(void)
{
    return (void *)&global_const;
}

/**
 * @brief Safe heap allocation with manual zeroing.
 */
void alloc_data_heap_segment(char **p_buffer, uint32_t size)
{
    if (p_buffer == NULL || size == 0U)
    {
        return;
    }

    *p_buffer = (char *)malloc(size);
    if (*p_buffer != NULL)
    {
        for (uint32_t i = 0U; i < size; ++i)
        {
            (*p_buffer)[i] = 0;
        }
    }
    else
    {
        (void)fprintf(stderr, "Error: malloc failed\n");
    }
}

/**
 * @brief Print memory map and distances.
 */
void print_memory_map(void)
{
    uint32_t stack_var = 0U;
    char *p_heap_buffer = NULL;

    alloc_data_heap_segment(&p_heap_buffer, HEAP_BUFFER_SIZE);

    printf("=== Memory Segment Map ===\n");

    print_segment("[TEXT]  ", "print_memory_map()", get_text_address());
    print_segment("[RODATA]", "global_const", get_rodata_address());
    print_segment("[DATA]  ", "global_init", &global_init);
    print_segment("[BSS]   ", "global_uninit", &global_uninit);

    if (p_heap_buffer != NULL)
    {
        print_segment("[HEAP]  ", "heap_buffer", p_heap_buffer);
    }
    else
    {
        printf("[HEAP]  heap allocation failed\n");
    }

    print_segment("[STACK] ", "stack_var", &stack_var);

    /* Custom section */
    printf("[CUSTOM] Start my_meta_data:      %p\n", (void *)&__start_my_meta_data);
    printf("[CUSTOM] Stop my_meta_data:       %p\n", (void *)&__stop_my_meta_data);
    printf("[CUSTOM] custom_var:              %p\n", (void *)&custom_var);

    /* Distances */
    uintptr_t addr_text   = (uintptr_t)get_text_address();
    uintptr_t addr_rodata = (uintptr_t)&global_const;
    uintptr_t addr_data   = (uintptr_t)&global_init;
    uintptr_t addr_bss    = (uintptr_t)&global_uninit;
    uintptr_t addr_heap   = (uintptr_t)p_heap_buffer;
    uintptr_t addr_stack  = (uintptr_t)&stack_var;

    printf("\n=== Segment Distances ===\n");
    printf("RODATA - TEXT:   %lu bytes\n", (addr_rodata > addr_text) ? addr_rodata - addr_text : addr_text - addr_rodata);
    printf("DATA   - RODATA: %lu bytes\n", (addr_data > addr_rodata) ? addr_data - addr_rodata : addr_rodata - addr_data);
    printf("BSS    - DATA:   %lu bytes\n", (addr_bss > addr_data) ? addr_bss - addr_data : addr_data - addr_bss);
    printf("HEAP   - BSS:    %lu bytes\n", (addr_heap > addr_bss) ? addr_heap - addr_bss : addr_bss - addr_heap);
    printf("STACK  - HEAP:   %lu bytes\n", (addr_stack > addr_heap) ? addr_stack - addr_heap : addr_heap - addr_stack);

    if (p_heap_buffer != NULL)
    {
        free(p_heap_buffer);
        p_heap_buffer = NULL;
    }
}

/**
 * @brief Main function
 */
int main(void)
{
    print_memory_map();
    return 0;
}



/**
 * @brief Output of main 
 * === Memory Segment Map ===
[TEXT]     print_memory_map()             0x62cd1fbc22d9
[RODATA]   global_const                   0x62cd1fbc3008
[DATA]     global_init                    0x62cd1fbc5010
[BSS]      global_uninit                  0x62cd1fbc506c
[HEAP]     heap_buffer                    0x62cd30c162a0
[STACK]    stack_var                      0x7ffd3b259bcc
[CUSTOM] Start my_meta_data:      0x62cd1fbc5020
[CUSTOM] Stop my_meta_data:       0x62cd1fbc5050
[CUSTOM] custom_var:              0x62cd1fbc5020

=== Segment Distances ===
RODATA - TEXT:   3375 bytes
DATA   - RODATA: 8200 bytes
BSS    - DATA:   92 bytes
HEAP   - BSS:    285545012 bytes
STACK  - HEAP:   32092169976108 bytes
 * 
 */


/**
 * @brief Output when run make verify
 * === size output ===
size main
   text    data     bss     dec     hex filename
   3695     692      24    4411    113b main
=== nm output for globals ===
nm main | grep -E 'global_(const|init|uninit)' || echo "Globals not found"
0000000000002008 R global_const
0000000000004010 D global_init
0000000000004070 B global_init_zero
000000000000406c B global_uninit
 */