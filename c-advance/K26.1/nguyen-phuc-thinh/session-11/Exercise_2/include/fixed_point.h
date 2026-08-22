/**
 * @file fixed_point.h
 * @brief Fixed-point arithmetic module (Q8.8 format).
 */
#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>
#include <stdbool.h>

#define Q8_8_SHIFT 8
#define Q8_8_SCALE (1 << Q8_8_SHIFT) /* 256 */

/** 
 * @brief Convert float to Q8.8 fixed-point. 
 *
 * @param[in] value Floating-point value to convert.
 * @return Q8.8 representation of the value.
 */
int16_t float_to_q8_8(float value);

/** 
 * @brief Convert Q8.8 fixed-point back to float (for display). 
 *
 * @param[in] q_value Q8.8 value to convert.
 * @return Floating-point representation.
 */
float q8_8_to_float(int16_t q_value);

/** 
 * @brief Multiply two Q8.8 values with 32-bit intermediate. 
 *
 * @param[in] a First Q8.8 operand.
 * @param[in] b Second Q8.8 operand.
 * @return Result of multiplication in Q8.8 format.
 */
int16_t q8_8_multiply(int16_t a, int16_t b);

/** 
 * @brief Check if a Q8.8 value exceeds a threshold. 
 *
 * @param[in] sample    Q8.8 sample value.
 * @param[in] threshold Q8.8 threshold value.
 * @return true if sample > threshold, false otherwise.
 */
bool q8_8_exceeds_threshold(int16_t sample, int16_t threshold);

#endif /* FIXED_POINT_H */