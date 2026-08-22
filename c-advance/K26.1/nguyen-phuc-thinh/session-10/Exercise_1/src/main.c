/**
 * @file main.c
 * @brief Demonstration of string memory placement and manipulation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_memory.h"

int main(void) 
{
    printf("========== L10 Lab 1: String Memory Inspector ==========\n\n");

    const char *p_literal_str = "firmware";
    char p_stack_str[] = "firmware";
    char *p_heap_str = heap_string_copy(p_literal_str);

    if (p_heap_str == NULL) 
    {
        return 1;
    }

    printf("--- Memory Locations ---\n");
    printf("Literal String Address (%s): %p\n", get_memory_region(p_literal_str), (const void*)p_literal_str);
    printf("Stack String Address   (%s)  : %p\n", get_memory_region(p_stack_str), (const void*)p_stack_str);
    printf("Heap String Address    (%s)   : %p\n\n", get_memory_region(p_heap_str), (const void*)p_heap_str);

    printf("--- String Modification ---\n");
    p_stack_str[0] = 'F';
    p_heap_str[0] = 'F';
    
    /* 
     * p_literal_str[0] = 'F'; 
     * DANGER: Uncommenting the line above causes Undefined Behavior (typically a Segmentation Fault).
     * String literals are stored in the Read-Only Data section (.rodata) and cannot be modified.
     */

    printf("Modified Stack String: %s\n", p_stack_str);
    printf("Modified Heap String : %s\n\n", p_heap_str);

    printf("--- sizeof() vs strlen() ---\n");
    printf("sizeof(p_stack_str) : %zu bytes (includes '\\0')\n", sizeof(p_stack_str));
    printf("strlen(p_stack_str) : %zu chars (excludes '\\0')\n\n", strlen(p_stack_str));

    printf("--- Missing Null Terminator ---\n");
    char p_bad_str[5] = {'h', 'e', 'l', 'l', 'o'};
    
    /* 
     * strlen expects a null terminator. Without it, it reads past the array bounds 
     * into adjacent memory until it randomly hits a 0x00 byte, returning a garbage value.
     */
    printf("strlen(bad_str) returned: %zu (Garbage value > 5!)\n\n", strlen(p_bad_str));

    free(p_heap_str);
    printf("Heap memory freed successfully.\n");

    return 0;
}