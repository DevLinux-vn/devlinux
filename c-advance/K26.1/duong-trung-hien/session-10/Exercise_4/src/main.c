#include "memory_ops.h"
#include <stdio.h>
#include <stdint.h>

#define APP_SUCCESS     (0)
#define BUFFER_SIZE     (8)

/**
 * @brief Prints a byte array in hexadecimal format.
 *
 * @param[in] p_data Pointer to the byte array to print.
 * @param[in] size   Number of bytes in the array.
 */
void print_hex_array(const uint8_t *p_data, size_t size);

/**
 * @brief Demonstrates the use of memset() for initializing or filling memory.
 */
void demo_memset(void);

/**
 * @brief Demonstrates the differences between memcpy() and strcpy().
 */
void demo_memcpy_vs_strcpy(void);

/**
 * @brief Demonstrates the differences between memcmp() and strcmp().
 */
void demo_memcmp_vs_strcmp(void);

/**
 * @brief Demonstrates the use of memmove() with overlapping memory regions.
 */
void demo_memmove(void);

int32_t main() {
    printf("========== L10 Lab 4: Memory vs String Operations ==========\n\n");

    demo_memset();
    demo_memcpy_vs_strcpy();
    demo_memcmp_vs_strcmp();
    demo_memmove();

    return APP_SUCCESS;
}

void print_hex_array(const uint8_t *p_data, size_t size) {
    for (uint32_t i = 0; i < size; i++) {                
        printf("%02X ", p_data[i]);           
    }  
}

void demo_memset(void) {
    sensor_data_t data;
    sensor_data_zero_init(&data);

    printf("--- memset ---\n");
    printf("Struct initialized to zeros.\n\n");
}

void demo_memcpy_vs_strcpy(void) {
    char dst[BUFFER_SIZE];
    uint8_t payload[BUFFER_SIZE] = {
    0x01, 0x00, 0xFF, 0x00,
    0x10, 0x20, 0x30, 0x40
    };

    printf("--- memcpy vs strcpy ---\n");
    printf("Payload: ");
    print_hex_array((const uint8_t *)payload, BUFFER_SIZE);

    binary_strncpy(dst, (const char *)&payload, BUFFER_SIZE);
    printf("\nstrncpy result: ");
    print_hex_array((const uint8_t *)dst, BUFFER_SIZE);
    printf(" (TRUNCATED at 0x00!)\n");

    binary_memcpy(dst, &payload, BUFFER_SIZE);
    printf("memcpy result : ");
    print_hex_array((const uint8_t *)dst, BUFFER_SIZE);
    printf(" (SUCCESS!)\n\n");
}

void demo_memcmp_vs_strcmp(void) {
    uint8_t id1[] = {0, 1, 0, 5};
    uint8_t id2[] = {0, 1, 0, 9};
    int32_t result1 = binary_strcmp((const char *)&id1, (const char *)&id2);
    int32_t result2 = binary_memcmp(&id1, &id2, sizeof(id1));
    
    printf("--- memcmp vs strcmp ---\n");
    printf("strcmp(\"0105\", \"0109\") -> %d (INCORRECT: strings match due to early 0x00)\n", result1);
    printf("memcmp(\"0105\", \"0109\") -> %d (CORRECT: arrays differ)\n", result2);
}

void demo_memmove(void) {
    printf("\n--- memmove ---\n");

    char buf[16] = "123456789";
    printf("Before shift: %s\n", buf);

    binary_memmove(&buf[5], &buf[3], 6);
    printf("After shift : %s\n", buf);
}