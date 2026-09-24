#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const uint32_t global_variable = 100;
uint32_t global_data = 50;
uint32_t global_uninit;

void print_memory_map(void)
{
    int local_init = 20;

    int *ptr;
    ptr = (int*)malloc(10*sizeof(int));

    if(ptr = NULL) 
    {
        return -1;
    }
    printf("TEXT address of main: %p\n", &print_memory_map);
    printf("RODATA address of CONST GLOBAL VARIABLE: %p\n", &global_variable);
    printf("DATA address of INITIALIZED GLOBAL VARIABLE: %p\n", &global_data);
    printf("BSS address of UNINITIALIZED GLOBAL VARIABLE: %p\n", &global_uninit);
    printf("HEAP address of PTR: %p\n", ptr);
    printf("STACK address of LOCAL VARIABLE: %p\n", &local_init);

    uintptr_t addr_main = (uintptr_t)&print_memory_map;
    uintptr_t addr_global_variable = (uintptr_t)&global_variable;
    uintptr_t addr_global_data = (uintptr_t)&global_data;
    uintptr_t addr_global_uninit = (uintptr_t)&global_uninit;
    uintptr_t addr_ptr = (uintptr_t)ptr;
    uintptr_t addr_local_init = (uintptr_t)&local_init;

    long long delta_1 = (long long)addr_global_variable - (long long)addr_main;
    printf("RODATA - TEXT = %lld byte\n", delta_1);

    long long delta_2 = (long long)addr_global_data - (long long)addr_global_variable;
    printf("DATA - TEXT = %lld byte\n", delta_2);

    long long delta_3 = (long long)addr_global_uninit - (long long)addr_global_data;
    printf("BSS - DATA = %lld byte\n", delta_3);

    long long delta_4 = (long long)addr_ptr - (long long)addr_global_uninit;
    printf("HEAP - BSS = %lld byte\n", delta_4);

    long long delta_5 = (long long)addr_local_init - (long long)addr_ptr;
    printf("STACK - HEAP = %lld byte\n", delta_5);


    free(ptr);
}

int main(){

    print_memory_map();
    return 0;
}

/*
    text	   data	    bss	    dec	    hex	filename
    2584	    628	     12	   3224	    c98	main

    0000000000004018 B global_uninit
    0000000000004010 D global_data
*/