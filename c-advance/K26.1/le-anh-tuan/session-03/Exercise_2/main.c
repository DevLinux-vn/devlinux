/**
 * @file main.c
 * @brief Demonstrates structure padding, member offsets, packed structs,
 *        and packed-member access risks.
 *
 * Structure padding exists because many CPUs access data more efficiently
 * when objects are placed at naturally aligned memory addresses. For example,
 * a uint32_t object is usually aligned to a 4-byte boundary. If a small member
 * appears before a larger member, the compiler may insert unused padding bytes
 * so the larger member starts at a suitable address.
 *
 * A packed structure removes this compiler-inserted padding. This reduces the
 * total size of the structure, which may be useful for communication protocols,
 * EEPROM layouts, flash records, and hardware register maps. However, packed
 * members may become unaligned.
 *
 * Direct access such as packed_object.counter is normally compiled safely
 * because the compiler knows the object is packed and can generate suitable
 * byte-wise accesses if needed. However, taking a pointer to a packed member
 * and accessing it as a normal uint32_t pointer can be dangerous. On some
 * embedded targets, especially ARM Cortex-M devices with strict alignment
 * requirements enabled, dereferencing an unaligned uint32_t pointer may cause
 * a HardFault.
 *
 * Therefore, packed structures should be used carefully. If multi-byte packed
 * data must be copied through a pointer, memcpy() is safer than dereferencing
 * a potentially unaligned pointer.
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**
 * @def PACKED
 * @brief Compiler-specific packed attribute.
 */
#define PACKED __attribute__((__packed__))

/**
 * @brief Original structure with inefficient member ordering.
 */
typedef struct
{
    uint8_t  status;
    uint32_t counter;
    uint16_t error_code;
    uint8_t  mode;
} unoptimized_data_t;

/**
 * @brief Optimized structure ordered from largest to smallest member.
 */
typedef struct
{
    uint32_t counter;
    uint16_t error_code;
    uint8_t  status;
    uint8_t  mode;
} optimized_data_t;

/**
 * @brief Packed version of the original unoptimized structure.
 */
typedef struct PACKED
{
    uint8_t  status;
    uint32_t counter;
    uint16_t error_code;
    uint8_t  mode;
} packed_data_t;

/**
 * @brief Prints a structure member offset.
 *
 * @param p_struct_name Name of the structure.
 * @param p_member_name Name of the member.
 * @param offset_bytes  Offset in bytes.
 */
static void print_offset(const char * const p_struct_name,
                         const char * const p_member_name,
                         const size_t offset_bytes)
{
    printf("%s.%s offset: %zu byte(s)\n",
           p_struct_name,
           p_member_name,
           offset_bytes);
}

/**
 * @brief Prints size and offsets of the unoptimized structure.
 */
static void print_unoptimized_layout(void)
{
    printf("Unoptimized structure\n");
    printf("sizeof(unoptimized_data_t): %zu byte(s)\n",
           sizeof(unoptimized_data_t));

    print_offset("unoptimized_data_t",
                 "status",
                 offsetof(unoptimized_data_t, status));
    print_offset("unoptimized_data_t",
                 "counter",
                 offsetof(unoptimized_data_t, counter));
    print_offset("unoptimized_data_t",
                 "error_code",
                 offsetof(unoptimized_data_t, error_code));
    print_offset("unoptimized_data_t",
                 "mode",
                 offsetof(unoptimized_data_t, mode));

    printf("\n");
}

/**
 * @brief Prints size and offsets of the optimized structure.
 */
static void print_optimized_layout(void)
{
    printf("Optimized structure\n");
    printf("sizeof(optimized_data_t): %zu byte(s)\n",
           sizeof(optimized_data_t));

    print_offset("optimized_data_t",
                 "counter",
                 offsetof(optimized_data_t, counter));
    print_offset("optimized_data_t",
                 "error_code",
                 offsetof(optimized_data_t, error_code));
    print_offset("optimized_data_t",
                 "status",
                 offsetof(optimized_data_t, status));
    print_offset("optimized_data_t",
                 "mode",
                 offsetof(optimized_data_t, mode));

    printf("\n");
}

/**
 * @brief Prints size and offsets of the packed structure.
 */
static void print_packed_layout(void)
{
    printf("Packed structure\n");
    printf("sizeof(packed_data_t): %zu byte(s)\n",
           sizeof(packed_data_t));

    print_offset("packed_data_t",
                 "status",
                 offsetof(packed_data_t, status));
    print_offset("packed_data_t",
                 "counter",
                 offsetof(packed_data_t, counter));
    print_offset("packed_data_t",
                 "error_code",
                 offsetof(packed_data_t, error_code));
    print_offset("packed_data_t",
                 "mode",
                 offsetof(packed_data_t, mode));

    printf("\n");
}

/**
 * @brief Demonstrates direct access and safe pointer-style access to packed data.
 *
 * @param p_data Pointer to packed structure object.
 */
static void demonstrate_packed_access(const packed_data_t * const p_data)
{
    const uint8_t * const p_raw_bytes = (const uint8_t *)p_data;
    uint32_t copied_counter = 0U;

    printf("Packed member access demonstration\n");

    printf("Direct packed access, p_data->counter: 0x%08" PRIX32 "\n",
           p_data->counter);

    (void)memcpy(&copied_counter,
                 &p_raw_bytes[offsetof(packed_data_t, counter)],
                 sizeof(copied_counter));

    printf("Safe byte-pointer plus memcpy access: 0x%08" PRIX32 "\n",
           copied_counter);

    printf("Danger note: do not cast the packed member address to uint32_t * "
           "and dereference it on embedded targets.\n");
    printf("Reason: the member may be unaligned, which can cause a HardFault.\n");
}

/**
 * @brief Program entry point.
 *
 * @return Returns 0 on success.
 */
int main(void)
{
    const packed_data_t packed_data =
    {
        .status = 0x11U,
        .counter = 0x22334455UL,
        .error_code = 0x6677U,
        .mode = 0x88U
    };

    print_unoptimized_layout();
    print_optimized_layout();
    print_packed_layout();
    demonstrate_packed_access(&packed_data);

    return 0;
}