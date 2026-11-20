/**
 * @file main.c
 * @brief Exercise 2 - Struct Padding, Alignment, and Packed Structs Analyzer
 */

#include <stddef.h> 
#include <stdint.h>
#include <stdio.h>

typedef struct {
    char   c; /* 1 byte  */
    int    i; /* 4 bytes */
    double d; /* 8 bytes */
    short  s; /* 2 bytes */
} unoptimized_t;

typedef struct {
    double d; /* 8 bytes */
    int    i; /* 4 bytes */
    short  s; /* 2 bytes */
    char   c; /* 1 byte */
} optimized_t;

typedef struct __attribute__((packed)) {
    char   c;
    int    i;
    double d;
    short  s;
} packed_t;

int main(void) {
    printf("=== Struct Padding Analyzer ===\n");

    printf("[Unoptimized Struct]\n");
    printf("Size: %zu bytes\n", sizeof(unoptimized_t));
    printf("Offsets: c(%zu), i(%zu), d(%zu), s(%zu)\n\n",
           offsetof(unoptimized_t, c),
           offsetof(unoptimized_t, i),
           offsetof(unoptimized_t, d),
           offsetof(unoptimized_t, s));

    printf("[Optimized Struct]\n");
    printf("Size: %zu bytes\n", sizeof(optimized_t));
    printf("Offsets: d(%zu), i(%zu), s(%zu), c(%zu)\n\n",
           offsetof(optimized_t, d),
           offsetof(optimized_t, i),
           offsetof(optimized_t, s),
           offsetof(optimized_t, c));

    printf("[Packed Struct]\n");
    printf("Size: %zu bytes\n", sizeof(packed_t));
    printf("Offsets: c(%zu), i(%zu), d(%zu), s(%zu)\n\n",
           offsetof(packed_t, c),
           offsetof(packed_t, i),
           offsetof(packed_t, d),
           offsetof(packed_t, s));

    packed_t my_packed = {0};

    printf("Attempting direct access to packed member... ");
    my_packed.i = 10;
    printf("Success! (Value = %d)\n", my_packed.i);

    printf("Attempting pointer access to packed member...(Warning generated during compilation)\n");

    // int *p_val = &my_packed.i; 
    // *p_val = 20;

    return 0;
}

/*
 * DISCUSSION TASK NUMBER 7:
// Q: Discuss the trade-offs and dangers of unaligned memory access (Hardware Faults on ARM) and why the pointer approach is unsafe.
// A: Unaligned memory access occurs when a program attempts to read or write data at a memory address that is not aligned to the size of the data type being accessed. 
 */