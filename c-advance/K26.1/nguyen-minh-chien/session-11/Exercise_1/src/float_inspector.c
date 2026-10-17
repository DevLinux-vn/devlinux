/**
 * @file    float_inspector.c
 * @brief   Implementation of IEEE-754 float bit-layout inspection,
 *          special-value classification, and epsilon comparison.
 */
#include "float_inspector.h"

#include <math.h>
#include <stdio.h>

/**
 * @brief Print the Sign, Exponent, and Mantissa fields of a float.
 *
 * @param[in] value The float value to inspect.
 */
void inspect_float(float value)
{
    float_inspector_t inspector;
    int32_t actual_exponent;

    inspector.f = value;

    /* Cast to int32_t before subtracting the bias so that biased
     * exponent values of 0 do not wrap around as unsigned. */
    actual_exponent = (int32_t)inspector.bits.exponent - FLOAT_INSPECTOR_EXPONENT_BIAS;

    (void)printf("Value: %-10f | Sign: %u | Exp: %-3u (Actual: %-4d) | Mantissa: 0x%06X\n",
                 (double)value,
                 inspector.bits.sign,
                 inspector.bits.exponent,
                 actual_exponent,
                 inspector.bits.mantissa);
}

/**
 * @brief Classify a float as a special IEEE-754 value or "Normal".
 *
 * @param[in] value The float value to classify.
 * @return A string classification (see header for full contract).
 */
const char *classify_float(float value)
{
    const char *p_result;

    /* MISRA Directive 4.11 / CERT FLP32-C: validate before treating
     * the value as "normal"; check special cases first using the
     * safe library predicates isnan()/isinf() rather than raw
     * comparisons against the value itself. */
    if (isnan(value) != 0)
    {
        p_result = "NaN";
    }
    else if (isinf(value) != 0)
    {
        p_result = (value > 0.0f) ? "+Infinity" : "-Infinity";
    }
    else
    {
        float_inspector_t inspector;

        inspector.f = value;

        /* Negative zero: sign bit set, exponent and mantissa both
         * zero. Checking bit fields avoids relying on `value == 0.0f`
         * where +0.0f and -0.0f would compare equal and hide the
         * distinction (MISRA Rule 13.3 rationale: keep float
         * comparisons out of branching logic where a bit-level check
         * is available and unambiguous). */
        if ((inspector.bits.sign == 1U) &&
            (inspector.bits.exponent == 0U) &&
            (inspector.bits.mantissa == 0U))
        {
            p_result = "Negative Zero";
        }
        else
        {
            p_result = "Normal";
        }
    }

    return p_result;
}

/**
 * @brief Compare two floats for approximate equality using epsilon.
 *
 * @param[in] a       First float operand.
 * @param[in] b       Second float operand.
 * @param[in] epsilon Tolerance threshold.
 * @return true if fabsf(a - b) < epsilon, false otherwise.
 */
bool float_equal(float a, float b, float epsilon)
{
    return (fabsf(a - b) < epsilon);
}