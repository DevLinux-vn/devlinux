#include "fixed_point.h"

int16_t float_to_q8_8(float value) {
    int32_t result = (int32_t)(value * Q8_8_SCALE);

    if (result > INT16_MAX) {
        result = INT16_MAX;
    } 
    else if (result < INT16_MIN) {
        result = INT16_MIN;
    }

    return (int16_t)result;
}

float q8_8_to_float(int16_t q_value) {
    return (float)(q_value / Q8_8_SCALE);
}

//Aq = A * 2^8, Bq = B * 2^8 => Aq * Bq = A * B * 2^16 => 16 bit fraction => >> 8 bit => Q8.8: 8 bit fraction
int16_t q8_8_multiply(int16_t a, int16_t b) {
    int32_t result = ((int32_t)a * (int32_t)b) >> Q8_8_SHIFT;
    
    if (result > INT16_MAX) {
        result = INT16_MAX;
    } 
    else if (result < INT16_MIN) {
        result = INT16_MIN;
    }

    return (int16_t)result;
}

bool q8_8_exceeds_threshold(int16_t sample, int16_t threshold) {
    if (sample > threshold) {
        return true;
    }

    return false;
}