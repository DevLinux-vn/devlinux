/**
 * @file main.c
 * @brief Main program demonstrating memory vs string operations.
 */
#include <stdio.h>
#include "memory_ops.h"

/**
 * @brief Helper function to print a byte array in hex format.
 */
static void print_hex_array(const uint8_t *p_arr, size_t size)
{
    for (size_t i = 0U; i < size; i++)
    {
        printf("%02X ", p_arr[i]);
    }
}

int main(void)
{
    printf("========== L10 Lab 4: Memory vs String Operations ==========\n\n");

    printf("--- memset ---\n");
    sensor_data_t sensor;
    init_sensor_data(&sensor);
    printf("Struct initialized to zeros.\n\n");

    printf("--- memcpy vs strncpy ---\n");
    uint8_t payload[8] = {0x01, 0x00, 0xFF, 0x00, 0x10, 0x20, 0x30, 0x40};
    uint8_t strncpy_dst[8] = {0};
    uint8_t memcpy_dst[8] = {0};

    printf("Payload: ");
    print_hex_array(payload, 8);
    printf("\n");

    copy_with_strncpy(strncpy_dst, payload, 8);
    printf("strncpy result: ");
    print_hex_array(strncpy_dst, 8);
    printf("(TRUNCATED at 0x00!)\n");

    copy_with_memcpy(memcpy_dst, payload, 8);
    printf("memcpy result : ");
    print_hex_array(memcpy_dst, 8);
    printf("(SUCCESS!)\n\n");

    printf("--- memcmp vs strcmp ---\n");
    uint8_t id1[] = {0, 1, 0, 5};
    uint8_t id2[] = {0, 1, 0, 9};
    
    int str_res = compare_with_strcmp(id1, id2);
    int mem_res = compare_with_memcmp(id1, id2, 4);
    
    printf("strcmp(\"0105\", \"0109\") -> %d (INCORRECT: strings match due to early 0x00)\n", str_res);
    printf("memcmp(\"0105\", \"0109\") -> %d (CORRECT: arrays differ)\n\n", mem_res);

    printf("--- memmove ---\n");
    char buf[16] = "123456789";
    printf("Before shift: %s\n", buf);
    
    /* Shift "456789" (6 chars + \0 = 7 bytes) two positions to the right. 
     * Offset 3 points to '4'. Dest offset 5 points to '6'. */
    shift_data_safely(buf, 3, 5, 7);
    
    printf("After shift : %s\n", buf);

    return 0;
}