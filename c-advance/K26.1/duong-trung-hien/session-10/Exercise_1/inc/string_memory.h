#ifndef STRING_MEMORY_H
#define STRING_MEMORY_H

#include <stdint.h>
#include <stddef.h>

#define HEAP_LABEL      "Heap Memory"
#define STACK_LABEL     "Stack Memory"
#define RODATA_LABEL    "Read-Only Data"

/**
 * @brief Determines the memory region containing a given address.
 *
 * This function uses the address range to heuristically determine whether
 * the pointer refers to stack memory, heap memory, or read-only data.
 *
 * @param[in] p_addr Pointer to the memory address to inspect.
 *
 * @return Memory region label:
 *         - "Stack" for stack memory.
 *         - "Heap" for heap memory.
 *         - "Read-Only Data" for read-only data.
 *         - "Unknown" if the region cannot be determined.
 *
 * @note The result is heuristic and may not be completely accurate due to
 *       memory layout, ASLR, and platform-specific behavior.
 */
const char* get_memory_region(const void *p_addr);

/**
 * @brief Creates a heap-allocated copy of a null-terminated string.
 *
 * Allocates sufficient memory on the heap to store a copy of the source
 * string, including the terminating null character.
 *
 * @param[in] p_src Source null-terminated string to copy.
 *
 * @return Pointer to the newly allocated string on success.
 * @return NULL if p_src is NULL or memory allocation fails.
 *
 * @note The caller is responsible for releasing the allocated memory
 *       using free().
 */
char* heap_string_copy(const char *p_src);

#endif