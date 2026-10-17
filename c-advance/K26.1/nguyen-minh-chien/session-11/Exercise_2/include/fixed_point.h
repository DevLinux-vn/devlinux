/**
 * @file    fixed_point.h
 * @brief   Q8.8 fixed-point arithmetic for real-time audio DSP on
 *          FPU-less microcontrollers (e.g. Cortex-M0).
 *
 * @details Q8.8 is a signed fixed-point format stored in a 16-bit
 *          integer: 8 bits for the integer part (including sign) and
 *          8 bits for the fractional part. A Q8.8 value is simply an
 *          ordinary integer that has been scaled by 256 (2^8); the
 *          "point" is purely conceptual; there is no hardware
 *          decimal point, only integers.
 *
 *          Representable range: approximately -128.0 to +127.996
 *          (i.e. INT16_MIN / 256 to INT16_MAX / 256). Any value whose
 *          integer part falls outside [-128, 127] cannot be
 *          represented in Q8.8 stored as int16_t.
 *
 *          Stages 2 (PROCESS) and 3 (DECIDE) of the pipeline defined
 *          in this exercise MUST NOT use float at all -- only
 *          int16_t / int32_t integer arithmetic -- so that this code
 *          can run on hardware with no FPU (CERT/MISRA-friendly,
 *          BARR-C style).
 */
#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Number of fractional bits in the Q8.8 format. */
#define Q8_8_SHIFT (8)

/** @brief Scale factor represented by one fractional-bit block (256). */
#define Q8_8_SCALE (1 << Q8_8_SHIFT)

/**
 * @brief Convert a floating-point value into Q8.8 fixed-point.
 *
 * @param[in] value The value to convert (e.g. a gain factor such as
 *                   1.75f).
 *
 * @return The Q8.8 representation of @p value, saturated
 *         (clamped) to [INT16_MIN, INT16_MAX] if @p value * 256
 *         would not fit in an int16_t.
 *
 * @note Per CERT-C INT31-C, converting an out-of-range float directly
 *       to int16_t is undefined behavior; this function saturates
 *       instead of casting blindly, so it is always safe to call.
 *       Use only at the ACQUIRE boundary (Stage 1) -- never inside
 *       the integer-only processing stages.
 */
int16_t float_to_q8_8(float value);

/**
 * @brief Convert a Q8.8 fixed-point value back into float.
 *
 * @param[in] q_value The Q8.8 value to convert.
 *
 * @return The floating-point equivalent of @p q_value (i.e.
 *         @p q_value / 256.0f).
 *
 * @note For display / logging purposes ONLY (Stage 4, OUTPUT). Must
 *       never be used inside the integer-only processing stages.
 */
float q8_8_to_float(int16_t q_value);

/**
 * @brief Multiply two Q8.8 fixed-point values.
 *
 * @param[in] a First Q8.8 operand.
 * @param[in] b Second Q8.8 operand.
 *
 * @return The Q8.8 product of @p a and @p b, saturated to
 *         [INT16_MIN, INT16_MAX] if the true product would not fit.
 *
 * @details Multiplying two Q8.8 values naively (`a * b`) produces a
 *          result scaled by 256*256 = 65536, which would overflow a
 *          16-bit integer almost immediately. Per CERT-C INT32-C,
 *          the multiplication is performed in a 32-bit intermediate,
 *          then shifted right by #Q8_8_SHIFT to rescale the product
 *          back down to a single factor of 256 (i.e. back to Q8.8).
 *          Entirely integer arithmetic -- no float involved.
 */
int16_t q8_8_multiply(int16_t a, int16_t b);

/**
 * @brief Check whether a Q8.8 sample exceeds a Q8.8 threshold.
 *
 * @param[in] sample    The Q8.8 sample to test.
 * @param[in] threshold The Q8.8 threshold to compare against.
 *
 * @return true  if @p sample is strictly greater than @p threshold.
 * @return false otherwise.
 *
 * @details Pure integer comparison -- no float involved, so this can
 *          run in a tight real-time interrupt handler on hardware
 *          with no FPU.
 */
bool q8_8_exceeds_threshold(int16_t sample, int16_t threshold);

/**
 * @brief Convert a raw 16-bit audio sample into Q8.8, detecting
 *        overflow instead of silently wrapping.
 *
 * @param[in]  raw_sample    The raw integer audio sample (e.g. from
 *                            a 16-bit ADC/PCM source).
 * @param[out] p_out_q8_8    On success, receives the Q8.8
 *                            representation of @p raw_sample
 *                            (i.e. raw_sample << #Q8_8_SHIFT).
 *
 * @return true  if @p raw_sample fits in Q8.8 (i.e.
 *               raw_sample is within [-128, 127]) and
 *               *p_out_q8_8 was written.
 * @return false if @p raw_sample is out of the representable Q8.8
 *               range, or if @p p_out_q8_8 is NULL. *p_out_q8_8 is
 *               left unmodified in this case.
 *
 * @details Q8.8 stored in int16_t can only represent integer
 *          magnitudes up to INT16_MAX / 256 ~= 127.996. Shifting a
 *          raw sample left by 8 bits is only safe if the sample's
 *          magnitude is small enough that the shifted result still
 *          fits in int16_t; otherwise the shift silently overflows
 *          (CERT-C INT31-C). This function performs the shift in a
 *          32-bit intermediate and checks the range BEFORE narrowing,
 *          so overflow is always detected rather than silently
 *          wrapped.
 */
bool sample_to_q8_8(int16_t raw_sample, int16_t *p_out_q8_8);

#ifdef __cplusplus
}
#endif

#endif /* FIXED_POINT_H */