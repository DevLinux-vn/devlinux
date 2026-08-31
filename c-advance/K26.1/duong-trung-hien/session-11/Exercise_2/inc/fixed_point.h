#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>
#include <stdbool.h>

#define Q8_8_SHIFT     (8)
#define Q8_8_SCALE     (1 << Q8_8_SHIFT)   /* 256 */

/**
 * @brief Convert a floating-point value to Q8.8 fixed-point format.
 *
 * Converts the specified floating-point value into a signed 16-bit Q8.8
 * fixed-point representation.
 *
 * @param[in] value Floating-point value to convert.
 *
 * @return The equivalent value in Q8.8 fixed-point format.
 */
int16_t float_to_q8_8(float value);

/**
 * @brief Convert a Q8.8 fixed-point value to floating-point format.
 *
 * @param[in] q_value Value represented in signed Q8.8 fixed-point format.
 *
 * @return The equivalent floating-point value.
 */
float q8_8_to_float(int16_t q_value);

/**
 * @brief Multiply two Q8.8 fixed-point values.
 *
 * Performs multiplication using a wider intermediate representation and
 * returns the result converted back to Q8.8 format.
 *
 * @param[in] a First Q8.8 fixed-point operand.
 * @param[in] b Second Q8.8 fixed-point operand.
 *
 * @return The multiplication result in Q8.8 fixed-point format.
 */
int16_t q8_8_multiply(int16_t a, int16_t b);

/**
 * @brief Check whether a Q8.8 sample exceeds a threshold.
 *
 * @param[in] sample    Q8.8 fixed-point sample value.
 * @param[in] threshold Q8.8 fixed-point threshold value.
 *
 * @return true if @p sample exceeds @p threshold; otherwise false.
 */
bool q8_8_exceeds_threshold(int16_t sample, int16_t threshold);

#endif /* FIXED_POINT_H */