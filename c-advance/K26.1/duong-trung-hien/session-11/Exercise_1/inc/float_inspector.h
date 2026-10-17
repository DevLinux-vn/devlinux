#ifndef FLOAT_INSPECTOR_H
#define FLOAT_INSPECTOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>  
#include <math.h>

#define INFINITE_CHECK    (0)

/**
 * @brief Union used to inspect the IEEE 754 single-precision representation
 *        of a floating-point value.
 *
 * This union allows the same 32-bit memory representation to be accessed as
 * a float value, an unsigned 32-bit integer, or individual IEEE 754 fields.
 *
 * @note The bit-field layout depends on compiler and platform implementation.
 */
typedef union {
    float    f; /**< Floating-point representation. */
    uint32_t u; /**< Raw 32-bit integer representation. */

    struct {
        uint32_t mantissa : 23; /**< Fraction (mantissa) field. */
        uint32_t exponent : 8;  /**< Exponent field. */
        uint32_t sign     : 1;  /**< Sign bit: 0 = positive, 1 = negative. */
    } bits; /**< IEEE 754 bit-field representation. */
} float_inspector_t;

/**
 * @brief Inspect the binary representation of a floating-point value.
 *
 * Extracts and stores the raw IEEE 754 representation and its individual
 * sign, exponent, and mantissa fields.
 *
 * @param[in]  value   Floating-point value to inspect.
 * @param[out] p_data  Pointer to the structure that receives the inspected
 *                     floating-point representation.
 *
 * @note The caller must provide a valid, non-NULL pointer.
 */
void inspect_float(float value, float_inspector_t *p_data);

/**
 * @brief Classify a floating-point value according to its IEEE 754 representation.
 *
 * Determines whether the specified floating-point value is a normal number,
 * zero, subnormal number, infinity, or NaN.
 *
 * @param[in]  value   Floating-point value to classify.
 * @param[out] p_data  Pointer to the structure containing or receiving the
 *                     inspected floating-point representation.
 *
 * @return A pointer to a constant string describing the classification.
 *
 * @note The caller must provide a valid, non-NULL pointer.
 */
const char *classify_float(float value, float_inspector_t *p_data);

/**
 * @brief Compare two floating-point values using an absolute tolerance.
 *
 * Determines whether the absolute difference between two floating-point
 * values is less than or equal to the specified epsilon.
 *
 * @param[in] a        First floating-point value.
 * @param[in] b        Second floating-point value.
 * @param[in] epsilon  Maximum allowed absolute difference.
 *
 * @return true if the absolute difference between @p a and @p b is less than
 *         or equal to @p epsilon; otherwise false.
 */
bool float_equal(float a, float b, float epsilon);

#endif /* FLOAT_INSPECTOR_H */