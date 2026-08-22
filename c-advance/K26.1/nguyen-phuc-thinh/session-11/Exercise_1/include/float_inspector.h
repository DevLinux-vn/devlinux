/**
 * @file float_inspector.h
 * @brief IEEE-754 Float inspection and safety validation API.
 */
#ifndef FLOAT_INSPECTOR_H
#define FLOAT_INSPECTOR_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Union for inspecting IEEE-754 float bit layout.
 */
typedef union {
    float f;
    uint32_t u;
    struct {
        uint32_t mantissa : 23;
        uint32_t exponent : 8;
        uint32_t sign     : 1;
    } bits;
} float_inspector_t;

/**
 * @brief Print the Sign, Exponent, and Mantissa of a float.
 * 
 * @param[in] value The float to inspect.
 */
void inspect_float(float value);

/**
 * @brief Classify a float as Normal, +Infinity, -Infinity, NaN, or Negative Zero.
 * 
 * @param[in] value The float to classify.
 * @return A string label for the float's classification.
 */
const char* classify_float(float value);

/**
 * @brief Compare two floats using epsilon tolerance.
 * 
 * @param[in] a First float.
 * @param[in] b Second float.
 * @param[in] epsilon Tolerance threshold.
 * @return true if |a - b| < epsilon, false otherwise.
 */
bool float_equal(float a, float b, float epsilon);

#endif /* FLOAT_INSPECTOR_H */