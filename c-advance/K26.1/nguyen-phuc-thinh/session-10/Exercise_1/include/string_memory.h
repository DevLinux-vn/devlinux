/**
 * @file string_memory.h
 * @brief Memory inspection and manipulation functions.
 */
#ifndef STRING_MEMORY_H
#define STRING_MEMORY_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Get the memory region label for a given pointer.
 *
 * @param[in] p_addr Pointer to inspect.
 * @return "Stack Memory", "Heap Memory", or "Read-Only Data" (heuristic based on address range).
 */
const char* get_memory_region(const void *p_addr);

/**
 * @brief Safely copy a string to a heap-allocated buffer.
 *
 * @param[in] p_src Source string (null-terminated).
 * @return Heap-allocated copy of the string, or NULL on failure. Caller must free().
 */
char* heap_string_copy(const char *p_src);

#endif /* STRING_MEMORY_H */