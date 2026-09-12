/**
 * @file main.c
 * @brief Memory Segment Analyzer exercise.
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define GLOBAL_CONST_VALUE (100U)
#define GLOBAL_INIT_VALUE  (42U)
#define HEAP_INIT_VALUE    (7U)
#define STACK_INIT_VALUE   (1U)

/**
 * @brief Function pointer type used as a reference to the text segment.
 */
typedef void (*memory_map_function_t)(void);

/**
 * @brief Stores two addresses for distance calculation.
 */
typedef struct
{
    uintptr_t first_address;
    uintptr_t second_address;
} segment_pair_t;

const uint32_t global_const = GLOBAL_CONST_VALUE;
uint32_t global_init = GLOBAL_INIT_VALUE;
uint32_t global_uninit;

static uintptr_t calculate_absolute_distance(segment_pair_t address_pair);
static void print_segment_distance(const char * p_label,
                                   segment_pair_t address_pair);
static void *get_text_display_address(memory_map_function_t p_function);

/**
 * @brief Prints addresses of variables in major memory segments.
 */
void print_memory_map(void)
{
    memory_map_function_t p_text_function = &print_memory_map;
    uint32_t *p_heap_var = NULL;
    uint32_t stack_var = STACK_INIT_VALUE;

    uintptr_t text_address = (uintptr_t)p_text_function;
    uintptr_t rodata_address = (uintptr_t)&global_const;
    uintptr_t data_address = (uintptr_t)&global_init;
    uintptr_t bss_address = (uintptr_t)&global_uninit;
    uintptr_t heap_address = 0U;
    uintptr_t stack_address = (uintptr_t)&stack_var;

    p_heap_var = malloc(sizeof(*p_heap_var));
    if (p_heap_var == NULL)
    {
        (void)fprintf(stderr, "Error: malloc failed.\n");
        exit(EXIT_FAILURE);
    }

    *p_heap_var = HEAP_INIT_VALUE;
    heap_address = (uintptr_t)p_heap_var;

    printf("=== Memory Segment Map ===\n");
    printf("[TEXT]   Address of print_memory_map(): %p\n",
           get_text_display_address(p_text_function));
    printf("[RODATA] Address of global_const:       %p\n",
           (const void *)&global_const);
    printf("[DATA]   Address of global_init:        %p\n",
           (void *)&global_init);
    printf("[BSS]    Address of global_uninit:      %p\n",
           (void *)&global_uninit);
    printf("[HEAP]   Address of heap_var:           %p\n",
           (void *)p_heap_var);
    printf("[STACK]  Address of stack_var:          %p\n",
           (void *)&stack_var);

    printf("\n=== Segment Distances ===\n");
    print_segment_distance("RODATA - TEXT:  ",
                           (segment_pair_t){rodata_address, text_address});
    print_segment_distance("DATA   - RODATA:",
                           (segment_pair_t){data_address, rodata_address});
    print_segment_distance("BSS    - DATA:  ",
                           (segment_pair_t){bss_address, data_address});
    print_segment_distance("HEAP   - BSS:   ",
                           (segment_pair_t){heap_address, bss_address});
    print_segment_distance("STACK  - HEAP:  ",
                           (segment_pair_t){stack_address, heap_address});

    free(p_heap_var);
    p_heap_var = NULL;
}

/**
 * @brief Calculates the absolute distance between two memory addresses.
 *
 * @param address_pair Pair of addresses to compare.
 *
 * @return Absolute distance in bytes.
 */
static uintptr_t calculate_absolute_distance(segment_pair_t address_pair)
{
    uintptr_t distance = 0U;

    if (address_pair.first_address >= address_pair.second_address)
    {
        distance = address_pair.first_address - address_pair.second_address;
    }
    else
    {
        distance = address_pair.second_address - address_pair.first_address;
    }

    return distance;
}

/**
 * @brief Prints a formatted segment distance line.
 *
 * @param p_label Label to print before the distance.
 * @param address_pair Pair of addresses to compare.
 */
static void print_segment_distance(const char * const p_label,
                                   segment_pair_t address_pair)
{
    printf("%s %" PRIuPTR " bytes\n",
           p_label,
           calculate_absolute_distance(address_pair));
}

/**
 * @brief Converts a function pointer to a display pointer for printf.
 *
 * @param p_function Function pointer used as text segment reference.
 *
 * @return Pointer value for display with %p.
 */
static void *get_text_display_address(memory_map_function_t p_function)
{
    void *p_address = NULL;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    p_address = (void *)p_function;
#pragma GCC diagnostic pop

    return p_address;
}

/**
 * @brief Program entry point.
 *
 * @return EXIT_SUCCESS on success.
 */
int main(void)
{
    print_memory_map();

    return EXIT_SUCCESS;
}

/*
Verification output example.

Commands:
size memory_map
nm memory_map | grep -i global_init
nm memory_map | grep -i global_uninit
nm memory_map | grep -i global_const

Example output:
   text    data     bss     dec     hex filename
   3233     628      16    3877     f25 memory_map
0000000000004040 D global_init
000000000000406c B global_uninit
0000000000002008 R global_const

Note:
Your exact numbers and addresses will be different because of compiler,
linker, architecture, and ASLR.
*/