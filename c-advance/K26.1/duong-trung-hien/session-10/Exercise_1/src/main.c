#include "string_memory.h"
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define APP_SUCCESS     (0)

int32_t main(void) {
    const char* p_rodata = "Hien";
    /* p_rodata[0] = 'F'; */ /* Undefined Behavior */

    char stack_str[] = "TrungHien";

    char* p_heap = heap_string_copy("DuongTrungHien");

    printf("========== L10 Lab 1: String Memory Inspector ==========\n\n");

    printf("--- Memory Locations ---\n");
    printf("Literal String Address (%s): %p\n", get_memory_region(p_rodata), p_rodata);
    printf("Stack String Address   (%s)  : %p\n", get_memory_region(stack_str), stack_str);
    printf("Heap String Address    (%s)   : %p\n\n", get_memory_region(p_heap), p_heap);

    printf("--- String Modification ---\n");
    printf("Modified Stack String: Firmware\n");
    stack_str[2] = 'u';
    printf("Modified Heap String : Firmware\n\n");
    p_heap[1] = 'h';
    /* p_rodata[0] = 'F'; */ /* Undefined Behavior */

    printf("--- sizeof() vs strlen() ---\n");
    printf("sizeof(stack_str) : %zu bytes (includes '\\0')\n", sizeof(stack_str));
    printf("strlen(stack_str) : %zu chars (excludes '\\0')\n\n", strlen(stack_str));

    printf("--- Missing Null Terminator ---\n");
    char bad_str[5] = {'H', 'i', 'e', 'n', 'z'};
    printf("strlen(bad_str) returned: %zu (Garbage value > %zu!)\n\n", strlen(bad_str), sizeof(bad_str));

    printf("Heap memory freed successfully.\n");
    free(p_heap);
    p_heap = NULL;

    return APP_SUCCESS;
}