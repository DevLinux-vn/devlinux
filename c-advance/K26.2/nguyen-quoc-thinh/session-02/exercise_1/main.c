#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <dlfcn.h>

const uint32_t global_const = 100U;
uint32_t global_init = 42U;
uint32_t global_uninit;

/**
 * @brief Calculate the absolute difference between two address values.
 *
 * @param addr1 First address value.
 * @param addr2 Second address value.
 *
 * @return Absolute difference between addr1 and addr2.
 */

uintptr_t address_distance(uintptr_t addr1, uintptr_t addr2)
{
    if (addr1 >= addr2)
    {
        return addr1 - addr2;
    }

    return addr2 - addr1;
}

/**
 * @brief Print representative memory addresses and their distances.
 */
void print_memory_map(void)
{
    uint32_t local_var = 50U;
    uint32_t *p_heap = malloc(sizeof(uint32_t));

    if (p_heap == NULL)
    {
        printf("Memory allocation failed\n");
        return;
    }

    /* Get function address from ELF symbol */
    void *p_handle = dlopen(NULL, RTLD_NOW);

    if (p_handle == NULL)
    {
        printf("dlopen failed\n");
        free(p_heap);
        return;
    }

    void *p_text = dlsym(p_handle, "print_memory_map");

    if (p_text == NULL)     
    {
        printf("dlsym failed\n");
        (void)dlclose(p_handle);
        free(p_heap);
        return;
    }

    /* Print memory addresses */
    printf("TEXT:   %p\n", p_text);
    printf("RODATA: %p\n", (void *)&global_const);
    printf("DATA:   %p\n", (void *)&global_init);
    printf("BSS:    %p\n", (void *)&global_uninit);
    printf("HEAP:   %p\n", (void *)p_heap);
    printf("STACK:  %p\n", (void *)&local_var);

    /* Convert addresses to integers */
    uintptr_t text_addr = (uintptr_t)p_text;
    uintptr_t rodata_addr = (uintptr_t)&global_const;
    uintptr_t data_addr = (uintptr_t)&global_init;
    uintptr_t bss_addr = (uintptr_t)&global_uninit;
    uintptr_t heap_addr = (uintptr_t)p_heap;
    uintptr_t stack_addr = (uintptr_t)&local_var;

    /* Calculate address distances */
    printf("\nMemory distances:\n");

    printf("TEXT -> RODATA: %" PRIuPTR " bytes\n",
           address_distance(text_addr, rodata_addr));

    printf("RODATA -> DATA: %" PRIuPTR " bytes\n",
           address_distance(rodata_addr, data_addr));

    printf("DATA -> BSS: %" PRIuPTR " bytes\n",
           address_distance(data_addr, bss_addr));

    printf("BSS -> HEAP: %" PRIuPTR " bytes\n",
           address_distance(bss_addr, heap_addr));

    printf("HEAP -> STACK: %" PRIuPTR " bytes\n",
           address_distance(heap_addr, stack_addr));

    /* Release resources */
    (void)dlclose(p_handle);
    free(p_heap);
}

int main(void)
{
    print_memory_map();
    return 0;
}

/*
 * Memory verification
 *
 * Command: size memory_map
 *
 * text    data    bss     dec     hex     filename
 * 3663    660     12      4335    10ef    memory_map
 *
 * Command: nm memory_map | grep -E 'global_init|global_uninit'
 *
 * 0000000000004010 D global_init
 * 0000000000004018 B global_uninit
 *
 * Verification:
 * D indicates initialized global data.
 * B indicates zero-initialized or uninitialized global data.
 *
 * Address distance between global_init and global_uninit:
 * 0x4018 - 0x4010 = 8 bytes.
 */
