/**
 * @file    main.c
 * @brief   Demo program: string memory placement, sizeof vs strlen,
 *          and the missing-null-terminator pitfall.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_memory.h"

/**
 * @brief   Program entry point.
 *
 * @return  0 on success, non-zero if heap allocation fails.
 */
int main(void)
{
    static const char *p_literal_str = "firmware";
    char stack_str[] = "firmware";
    char bad_str[5];
    char *p_heap_str = NULL;

    printf("========== L10 Lab 1: String Memory Inspector ==========\n\n");

    p_heap_str = heap_string_copy(p_literal_str);
    if (p_heap_str == NULL)
    {
        (void)fprintf(stderr, "Heap allocation failed.\n");
        return 1;
    }

    /* Record one known address per region so get_memory_region() can
     * classify other addresses by proximity (see string_memory.c). */
    memory_region_set_references((const void *)p_literal_str,
                                  (const void *)stack_str,
                                  (const void *)p_heap_str);

    printf("--- Memory Locations ---\n");
    printf("Literal String Address (%s): %p\n",
           get_memory_region((const void *)p_literal_str), (const void *)p_literal_str);
    printf("Stack String Address   (%s)  : %p\n",
           get_memory_region((const void *)stack_str), (const void *)stack_str);
    printf("Heap String Address    (%s)  : %p\n\n",
           get_memory_region((const void *)p_heap_str), (const void *)p_heap_str);

    printf("--- String Modification ---\n");
    stack_str[0] = 'F';
    p_heap_str[0] = 'F';
    printf("Modified Stack String: %s\n", stack_str);
    printf("Modified Heap String : %s\n\n", p_heap_str);

    /* CERT STR30-C: string literals live in read-only data (Flash on
     * embedded targets). Writing to one is Undefined Behavior and
     * typically crashes (SIGSEGV) or corrupts firmware. NEVER
     * uncomment the line below:
     *
     *   p_literal_str[0] = 'F';
     */

    printf("--- sizeof() vs strlen() ---\n");
    printf("sizeof(stack_str) : %zu bytes (includes '\\0')\n", sizeof(stack_str));
    printf("strlen(stack_str) : %zu chars (excludes '\\0')\n\n", strlen(stack_str));

    printf("--- Missing Null Terminator ---\n");
    bad_str[0] = 'H';
    bad_str[1] = 'e';
    bad_str[2] = 'l';
    bad_str[3] = 'l';
    bad_str[4] = 'o';
    /* No '\0' was written. strlen() will keep scanning past the array
     * bounds until it happens to find a zero byte somewhere else in
     * memory: this is Undefined Behavior (CERT EXP33-C, MISRA 9.1).
     * The value printed below is NOT reliable and will vary between
     * runs, compilers, or machines. */
    printf("strlen(bad_str) returned: %zu (Garbage value - undefined behavior!)\n\n",
           strlen(bad_str));

    free(p_heap_str);
    p_heap_str = NULL;
    printf("Heap memory freed successfully.\n");

    return 0;
}