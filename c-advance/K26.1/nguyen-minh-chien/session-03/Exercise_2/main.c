#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
static const int32_t DIRECT_ACCESS_VALUE = 10;
static const int32_t POINTER_ACCESS_VALUE = 20;
/**
 * @brief Unoptimized struct with members in declaration order,
 *        causing extra padding between members.
 */
typedef struct {
    char   c;      // 1 byte
    int    i;      // 4 bytes
    double d;      // 8 bytes
    short  s;      // 2 bytes
} unoptimized_t;

/**
 * @brief Optimized struct with members reordered from largest to
 *        smallest, minimizing padding.
 */
typedef struct {
    double d;
    int    i;
    short  s;
    char   c;
} optimized_t;

/**
 * @brief Packed struct that forces the compiler to remove all
 *        padding, at the cost of unaligned member access.
 */
typedef struct __attribute__((packed)) {
    char   c;
    int    i;
    double d;
    short  s;
} packed_t;

/**
 * @brief Entry point. Prints size/offset info for each struct
 *        variant and demonstrates direct vs pointer access on a
 *        packed struct member.
 *
 * @details
 * Direct access (my_int.i = 10) is safe: the compiler knows the
 * struct is packed and generates byte-by-byte read/write code to
 * handle the unaligned member correctly, at the cost of speed.
 *
 * Pointer access (int *p_i = &my_int.i) is dangerous: once the
 * address is stored in a plain int*, the compiler assumes it is
 * 4-byte aligned, as required by the C standard. Dereferencing it
 * may generate a single aligned load/store instruction. On ARM
 * hardware, executing that instruction on an unaligned address can
 * trigger a HardFault (hardware crash), or silently produce wrong
 * data on architectures that tolerate unaligned access. This is why
 * GCC/Clang warn with -Waddress-of-packed-member.
 *
 * @return 0 on success.
 */

int main ( void ) {
    printf("=== Struct Padding Analyzer ===\n");
    printf("[Unoptimized Struct]\n");
    printf("Size of unoptimized: %zu\n",sizeof(unoptimized_t));
    printf("Offsets: c(%zu), i(%zu), d(%zu), s(%zu)\n",offsetof(unoptimized_t, c),offsetof(unoptimized_t, i),offsetof(unoptimized_t, d),offsetof(unoptimized_t, s));
    printf("[Optimized Struct]\n");
    printf("Size of optimized: %zu\n",sizeof(optimized_t));
    printf("Offsets: d(%zu), i(%zu), s(%zu), c(%zu)\n",offsetof(optimized_t, d),offsetof(optimized_t, i),offsetof(optimized_t, s),offsetof(optimized_t, c));
    printf("[Packed Struct]\n");
    printf("Size of packed: %zu\n",sizeof(packed_t));
    printf("Offsets: c(%zu), i(%zu), d(%zu), s(%zu)\n",offsetof(packed_t, c),offsetof(packed_t, i),offsetof(packed_t, d),offsetof(packed_t, s));
    packed_t my_int ={0};
    my_int.i = DIRECT_ACCESS_VALUE;
    printf("Attempting direct access to packed member... Success! (i = %d)\n", my_int.i);
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Waddress-of-packed-member"
    int *p_i = &my_int.i;
    #pragma GCC diagnostic pop

    *p_i = POINTER_ACCESS_VALUE;
    printf("Attempting pointer access to packed member... (Warning generated during compilation) (i = %d)\n", *p_i);
    return 0;
}