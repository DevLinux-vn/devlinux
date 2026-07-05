
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

/**
 * @brief Application success return code.
 */
#define APP_SUCCESS (0)

/**
 * @brief Original unoptimized struct layout.
 */
typedef struct
{
    char c;
    int32_t i;
    double d;
    int16_t s;
} unoptimized_t;

/**
 * @brief Optimized struct layout ordered from largest to smallest type.
 */
typedef struct
{
    double d;
    int32_t i;
    int16_t s;
    char c;
} optimized_t;

/**
 * @brief Packed version of the original struct layout.
 */
typedef struct __attribute__((packed))
{
    char c;
    int32_t i;
    double d;
    int16_t s;
} packed_t;

/**
 * @brief Print layout information for the unoptimized struct.
 */
static void print_unoptimized_layout(void)
{
    (void)printf("[Unoptimized Struct]\n");
    (void)printf("Size: %zu bytes\n", sizeof(unoptimized_t));
    (void)printf("Offsets: c(%zu), i(%zu), d(%zu), s(%zu)\n\n",
                 offsetof(unoptimized_t, c),
                 offsetof(unoptimized_t, i),
                 offsetof(unoptimized_t, d),
                 offsetof(unoptimized_t, s));
}

/**
 * @brief Print layout information for the optimized struct.
 */
static void print_optimized_layout(void)
{
    (void)printf("[Optimized Struct]\n");
    (void)printf("Size: %zu bytes\n", sizeof(optimized_t));
    (void)printf("Offsets: d(%zu), i(%zu), s(%zu), c(%zu)\n\n",
                 offsetof(optimized_t, d),
                 offsetof(optimized_t, i),
                 offsetof(optimized_t, s),
                 offsetof(optimized_t, c));
}

/**
 * @brief Print layout information for the packed struct.
 */
static void print_packed_layout(void)
{
    (void)printf("[Packed Struct]\n");
    (void)printf("Size: %zu bytes\n", sizeof(packed_t));
    (void)printf("Offsets: c(%zu), i(%zu), d(%zu), s(%zu)\n\n",
                 offsetof(packed_t, c),
                 offsetof(packed_t, i),
                 offsetof(packed_t, d),
                 offsetof(packed_t, s));
}

/**
 * @brief Demonstrate direct access and safe byte-copy access for packed data.
 */
static void demonstrate_packed_access(void)
{
    packed_t packed_data = { 0 };
    int32_t temp_value = 20;

    (void)printf("Attempting direct access to packed member... ");
    packed_data.i = 10;
    (void)printf("Success! value = %" PRId32 "\n", packed_data.i);

    /*
     * Unsafe example required by the exercise:
     *
     *     int32_t *p_i = &packed_data.i;
     *     *p_i = 20;
     *
     * This can generate a compiler warning such as:
     * "taking address of packed member may result in an unaligned pointer value".
     * With -Werror, that warning becomes a build error. Therefore, this unsafe
     * code is intentionally not compiled. A safer method is to copy bytes with
     * memcpy or assign the member directly.
     */

    (void)printf("Attempting pointer access to packed member... skipped intentionally.\n");

    packed_data.i = temp_value;
    (void)printf("Safe direct assignment instead. value = %" PRId32 "\n", packed_data.i);
}


/*Comment ouput when run main
=== Struct Padding Analyzer ===

[Unoptimized Struct]
Size: 24 bytes
Offsets: c(0), i(4), d(8), s(16)

[Optimized Struct]
Size: 16 bytes
Offsets: d(0), i(8), s(12), c(14)

[Packed Struct]
Size: 15 bytes
Offsets: c(0), i(1), d(5), s(13)

Attempting direct access to packed member... Success! value = 10
Attempting pointer access to packed member... skipped intentionally.
Safe direct assignment instead. value = 20*/



/**
 * @brief Program entry point.
 *
 * @return APP_SUCCESS when the program completes successfully.
 */
int main(void)
{
    (void)printf("=== Struct Padding Analyzer ===\n\n");

    print_unoptimized_layout();
    print_optimized_layout();
    print_packed_layout();
    demonstrate_packed_access();

    return APP_SUCCESS;
}
