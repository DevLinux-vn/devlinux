/**
 * @file    fixed_point.c
 * @brief   Implementation of Q8.8 fixed-point conversion and
 *          arithmetic for the FPU-less audio DSP pipeline.
 */
#include "fixed_point.h"

#include <stddef.h>

/**
 * @brief Convert a float into Q8.8, saturating on overflow.
 *
 * @param[in] value The value to convert.
 * @return Saturated Q8.8 representation of @p value.
 */
int16_t float_to_q8_8(float value)
{
    /* Scale in float first (Stage 1 is the one place float is
     * allowed). */
    float scaled = value * (float)Q8_8_SCALE;
    int16_t result;

    /* CERT INT31-C: converting a float outside int16_t's range
     * directly to int16_t is undefined behavior. Saturate instead of
     * casting blindly. */
    if (scaled > (float)INT16_MAX)
    {
        result = INT16_MAX;
    }
    else if (scaled < (float)INT16_MIN)
    {
        result = INT16_MIN;
    }
    else
    {
        result = (int16_t)scaled;
    }

    return result;
}

/**
 * @brief Convert a Q8.8 value back to float, for display only.
 *
 * @param[in] q_value The Q8.8 value to convert.
 * @return Floating-point equivalent of @p q_value.
 */
float q8_8_to_float(int16_t q_value)
{
    return (float)q_value / (float)Q8_8_SCALE;
}

/**
 * @brief Multiply two Q8.8 values using a 32-bit intermediate.
 *
 * @param[in] a First Q8.8 operand.
 * @param[in] b Second Q8.8 operand.
 * @return Saturated Q8.8 product of @p a and @p b.
 */
int16_t q8_8_multiply(int16_t a, int16_t b)
{
    /* CERT INT32-C: widen to int32_t BEFORE multiplying, so the
     * a*b product (scaled by 256*256) cannot overflow. */
    int32_t product = (int32_t)a * (int32_t)b;

    /* MISRA Rule 12.1: parenthesize explicitly even though shift
     * binds correctly here, to make precedence unambiguous. */
    int32_t rescaled = (product >> Q8_8_SHIFT);
    int16_t result;

    /* CERT INT31-C: validate range before narrowing to int16_t. */
    if (rescaled > (int32_t)INT16_MAX)
    {
        result = INT16_MAX;
    }
    else if (rescaled < (int32_t)INT16_MIN)
    {
        result = INT16_MIN;
    }
    else
    {
        result = (int16_t)rescaled;
    }

    return result;
}

/**
 * @brief Compare two Q8.8 values in the pure integer domain.
 *
 * @param[in] sample    Q8.8 sample.
 * @param[in] threshold Q8.8 threshold.
 * @return true if @p sample > @p threshold.
 */
bool q8_8_exceeds_threshold(int16_t sample, int16_t threshold)
{
    return (sample > threshold);
}

/**
 * @brief Convert a raw integer sample to Q8.8, detecting overflow.
 *
 * @param[in]  raw_sample  Raw audio sample.
 * @param[out] p_out_q8_8  Receives the Q8.8 result on success.
 * @return true on success, false on overflow or NULL output pointer.
 */
bool sample_to_q8_8(int16_t raw_sample, int16_t *p_out_q8_8)
{
    bool converted = false;

    if (p_out_q8_8 != NULL)
    {
        /* Widen to int32_t before shifting so the shift itself never
         * overflows; only the final narrowing needs a range check
         * (CERT INT31-C). */
        int32_t shifted = ((int32_t)raw_sample << Q8_8_SHIFT);

        if ((shifted >= (int32_t)INT16_MIN) && (shifted <= (int32_t)INT16_MAX))
        {
            *p_out_q8_8 = (int16_t)shifted;
            converted = true;
        }
    }

    return converted;
}