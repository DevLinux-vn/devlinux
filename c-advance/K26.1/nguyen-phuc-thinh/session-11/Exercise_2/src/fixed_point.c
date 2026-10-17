/**
 * @file fixed_point.c
 * @brief Implementation of Q8.8 fixed-point math operations.
 */
#include "fixed_point.h"

int16_t float_to_q8_8(float value)
{
    return (int16_t)(value * (float)Q8_8_SCALE);
}

float q8_8_to_float(int16_t q_value)
{
    return (float)q_value / (float)Q8_8_SCALE;
}

int16_t q8_8_multiply(int16_t a, int16_t b)
{
    /* Use 32-bit intermediate to prevent overflow (INT32-C), then shift back down */
    int32_t temp = (int32_t)a * (int32_t)b;
    return (int16_t)(temp >> Q8_8_SHIFT);
}

bool q8_8_exceeds_threshold(int16_t sample, int16_t threshold)
{
    /* Threshold comparison entirely in the integer domain */
    return (sample > threshold);
}