/**
 * @file    string_memory.h
 * @brief   Utilities to inspect and safely copy strings across the
 *          read-only data, stack, and heap memory regions.
 */
#ifndef STRING_MEMORY_H
#define STRING_MEMORY_H

#include <stddef.h>

/**
 * @brief   Record one known sample address from each memory region.
 *
 * @details Reliable region detection normally requires external tools
 *          (nm, objdump, gdb). As a practical heuristic usable from
 *          inside the program, this function records one address the
 *          caller already knows belongs to each region. Addresses
 *          within the same region are typically clustered close
 *          together in memory, so get_memory_region() can then guess
 *          an unknown address's region by finding which reference it
 *          is numerically closest to.
 *
 * @param[in] p_rodata_ref Address known to be in read-only data.
 * @param[in] p_stack_ref  Address known to be on the stack.
 * @param[in] p_heap_ref   Address known to be on the heap.
 */
void memory_region_set_references(const void *p_rodata_ref,
                                   const void *p_stack_ref,
                                   const void *p_heap_ref);

/**
 * @brief   Get the memory region label for a given pointer.
 *
 * @param[in] p_addr Pointer to inspect.
 *
 * @return  "Read-Only Data", "Stack", or "Heap" (a heuristic guess
 *          based on proximity to the references previously recorded
 *          via memory_region_set_references()), or "Unknown" if no
 *          references have been recorded yet.
 */
const char *get_memory_region(const void *p_addr);

/**
 * @brief   Safely copy a string into a newly heap-allocated buffer.
 *
 * @param[in] p_src Source string (null-terminated). May be NULL.
 *
 * @return  Heap-allocated copy of the string, or NULL if @p p_src is
 *          NULL or allocation fails. The caller must free() the
 *          returned pointer.
 */
char *heap_string_copy(const char *p_src);

#endif /* STRING_MEMORY_H */