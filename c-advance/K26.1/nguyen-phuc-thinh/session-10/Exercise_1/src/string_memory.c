/**
 * @file string_memory.c
 * @brief Implementation of memory inspection utilities.
 */
#include "string_memory.h"
#include <stdlib.h>
#include <string.h>

const char* get_memory_region(const void *p_addr) 
{
    if (p_addr == NULL) 
    {
        return "Unknown";
    }

    /* Heuristic references for different memory regions */
    int stack_dummy = 0;
    void *p_heap_dummy = malloc(1);
    const char *p_ro_dummy = "ro_data";

    uintptr_t addr = (uintptr_t)p_addr;
    uintptr_t stack_ref = (uintptr_t)&stack_dummy;
    uintptr_t heap_ref = (uintptr_t)p_heap_dummy;
    uintptr_t ro_ref = (uintptr_t)p_ro_dummy;

    if (p_heap_dummy != NULL) 
    {
        free(p_heap_dummy);
    }

    uintptr_t dist_stack = (addr > stack_ref) ? (addr - stack_ref) : (stack_ref - addr);
    uintptr_t dist_heap  = (addr > heap_ref)  ? (addr - heap_ref)  : (heap_ref - addr);
    uintptr_t dist_ro    = (addr > ro_ref)    ? (addr - ro_ref)    : (ro_ref - addr);

    if ((dist_stack < dist_heap) && (dist_stack < dist_ro)) 
    {
        return "Stack Memory";
    } 
    else if ((dist_heap < dist_stack) && (dist_heap < dist_ro)) 
    {
        return "Heap Memory";
    } 
    
    return "Read-Only Data";
}

char* heap_string_copy(const char *p_src) 
{
    if (p_src == NULL) 
    {
        return NULL;
    }
    
    size_t len = strlen(p_src);
    char *p_dest = (char *)malloc(len + 1); 
    
    if (p_dest != NULL) 
    {
        strcpy(p_dest, p_src);
    }
    
    return p_dest;
}