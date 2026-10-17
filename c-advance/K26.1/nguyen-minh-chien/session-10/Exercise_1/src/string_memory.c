/**
 * @file    string_memory.c
 * @brief   Implementation of string memory inspection utilities.
 */
#include "string_memory.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** @brief Reference address known to be in read-only data. */
static const void *s_ref_rodata = NULL;

/** @brief Reference address known to be on the stack. */
static const void *s_ref_stack = NULL;

/** @brief Reference address known to be on the heap. */
static const void *s_ref_heap = NULL;

/**
 * @brief   Compute the absolute difference between two addresses.
 *
 * @param[in] addr_a First address, as an integer.
 * @param[in] addr_b Second address, as an integer.
 *
 * @return  The absolute value of (addr_a - addr_b).
 */
static uintptr_t address_distance(uintptr_t addr_a, uintptr_t addr_b)
{
    uintptr_t distance;

    if (addr_a > addr_b)
    {
        distance = addr_a - addr_b;
    }
    else
    {
        distance = addr_b - addr_a;
    }

    return distance;
}

void memory_region_set_references(const void *p_rodata_ref,
                                   const void *p_stack_ref,
                                   const void *p_heap_ref)
{
    s_ref_rodata = p_rodata_ref;
    s_ref_stack = p_stack_ref;
    s_ref_heap = p_heap_ref;
}

const char *get_memory_region(const void *p_addr)
{
    const char *p_region = "Unknown";

    if ((s_ref_rodata != NULL) && (s_ref_stack != NULL) && (s_ref_heap != NULL))
    {
        uintptr_t addr = (uintptr_t)p_addr;
        uintptr_t dist_rodata = address_distance(addr, (uintptr_t)s_ref_rodata);
        uintptr_t dist_stack = address_distance(addr, (uintptr_t)s_ref_stack);
        uintptr_t dist_heap = address_distance(addr, (uintptr_t)s_ref_heap);

        if ((dist_rodata <= dist_stack) && (dist_rodata <= dist_heap))
        {
            p_region = "Read-Only Data";
        }
        else if (dist_stack <= dist_heap)
        {
            p_region = "Stack";
        }
        else
        {
            p_region = "Heap";
        }
    }

    return p_region;
}

char *heap_string_copy(const char *p_src)
{
    char *p_dst = NULL;

    if (p_src != NULL)
    {
        size_t len = strlen(p_src) + 1U;

        p_dst = (char *)malloc(len);

        if (p_dst != NULL)
        {
            (void)memcpy(p_dst, p_src, len);
        }
    }

    return p_dst;
}