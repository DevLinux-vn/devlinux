#ifndef STRUCT_PADDING_ANALYZER_H
#define STRUCT_PADDING_ANALYZER_H

#include <stdio.h>
#include <stddef.h> // For offsetof()
#include <stdlib.h> // For malloc()
#include <stdint.h> 

/**
 * @brief Unoptimized structure used to demonstrate compiler padding.
 */
typedef struct
{
    char c;     /**< Character member (1 byte). */
    int i;      /**< Integer member (4 bytes). */
    double d;   /**< Double-precision floating-point member (8 bytes). */
    short s;    /**< Short integer member (2 bytes). */
} unoptimized_t;

/**
 * @brief Optimized structure with minimized padding.
 *
 * Members are ordered from largest alignment requirement to smallest
 * in order to reduce compiler-inserted padding.
 */
typedef struct
{
    double d;  
    int i;      
    short s;  
    char c;   
} optimized_t;

/**
 * @brief Packed structure with padding removed.
 */
typedef struct __attribute__((packed))
{
    char c;    
    int i;     
    double d;   
    short s;    
} packed_t;

/**
 * @brief Print size and member offsets of the unoptimized structure.
 */
void print_unoptimized_info(void);

/**
 * @brief Print size and member offsets of the optimized structure.
 */
void print_optimized_info(void);

/**
 * @brief Print size and member offsets of the packed structure.
 */
void print_packed_info(void);

/**
 * @brief Demonstrate direct and pointer access to packed structure members.
 */
void demonstrate_packed_access(void);

#endif /* STRUCT_PADDING_ANALYZER_H */