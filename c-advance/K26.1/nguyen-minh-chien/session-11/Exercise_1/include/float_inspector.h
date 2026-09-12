/**
 * @file    float_inspector.h
 * @brief   IEEE-754 single-precision float bit-layout inspector,
 *          special-value classifier, and safe comparison utilities.
 *
 * @details This header declares the public API used to inspect the
 *          raw bit representation of IEEE-754 floats (sign, exponent,
 *          mantissa), classify special values (Infinity, NaN,
 *          Negative Zero, Normal), and safely compare floats using an
 *          epsilon tolerance rather than the exact `==` operator.
 */
#ifndef FLOAT_INSPECTOR_H
#define FLOAT_INSPECTOR_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Union used to inspect the raw bit layout of an IEEE-754
 *        single-precision float.
 *
 * @details All three members share the same 4 bytes of storage.
 *          Writing to `f` and reading `bits` (or `u`) lets us examine
 *          the sign, exponent, and mantissa fields without invoking
 *          undefined behavior via pointer type-punning.
 */
typedef union float_inspector_u
{
    float f;    /**< The float value, as normally used. */
    uint32_t u; /**< The same 32 bits, viewed as an unsigned integer. */

    /**
     * @brief Bit-field view of the IEEE-754 layout.
     *
     * @note Bit-field order/packing is compiler and platform defined
     *       by the C standard; this layout is verified to match the
     *       little-endian x86/x86_64/ARM targets used for this
     *       exercise's build and CI.
     */
    struct
    {
        uint32_t mantissa : 23; /**< Fraction / significand bits. */
        uint32_t exponent : 8;  /**< Biased exponent bits (bias = 127). */
        uint32_t sign : 1;      /**< Sign bit: 0 = positive, 1 = negative. */
    } bits;
} float_inspector_t;

/** @brief IEEE-754 single-precision exponent bias. */
#define FLOAT_INSPECTOR_EXPONENT_BIAS (127)

/**
 * @brief Print the Sign, Exponent (raw + unbiased), and Mantissa
 *        fields of a float in a human-readable, formatted line.
 *
 * @param[in] value The float value to inspect.
 *
 * @details Uses a #float_inspector_t union internally to safely
 *          extract the bit fields. Output format:
 *          `Value: <val> | Sign: <0|1> | Exp: <raw> (Actual: <unbiased>) | Mantissa: 0x<hex>`
 */
void inspect_float(float value);

/**
 * @brief Classify a float as a special IEEE-754 value or "Normal".
 *
 * @param[in] value The float value to classify.
 *
 * @return A pointer to a static, read-only string literal:
 *         - "+Infinity"     if value is positive infinity.
 *         - "-Infinity"     if value is negative infinity.
 *         - "NaN"           if value is Not-a-Number.
 *         - "Negative Zero" if value is exactly -0.0f (bit-level check).
 *         - "Normal"        otherwise.
 *
 * @note The returned pointer refers to string literals with static
 *       storage duration; it is always valid and MUST NOT be freed.
 */
const char *classify_float(float value);

/**
 * @brief Compare two floats for approximate equality using an
 *        epsilon tolerance, instead of the exact `==` operator.
 *
 * @param[in] a       First float operand.
 * @param[in] b       Second float operand.
 * @param[in] epsilon Tolerance threshold; must be a small positive
 *                     value (e.g. 1e-6f).
 *
 * @return true  if fabsf(a - b) < epsilon.
 * @return false otherwise.
 */
bool float_equal(float a, float b, float epsilon);

#ifdef __cplusplus
}
#endif

#endif /* FLOAT_INSPECTOR_H */