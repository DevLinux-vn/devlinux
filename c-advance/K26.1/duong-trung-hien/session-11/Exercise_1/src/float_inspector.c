#include "float_inspector.h"

void inspect_float(float value, float_inspector_t *p_data) {
    p_data->f = value;
}

const char* classify_float(float value, float_inspector_t *p_data) {
    if (value > 0) {
        if (isinf(value) != INFINITE_CHECK) {
            return "+Infinity";
        }
    }
    else if(value < 0) {
        if (isinf(value) != INFINITE_CHECK) {
            return "-Infinity";
        }
    }

    if (isnan(value) != INFINITE_CHECK) {
        return "NaN";
    }

    p_data->f = value;
    if ((p_data->bits.sign == 1U) &&
        (p_data->bits.exponent == 0U) &&
        (p_data->bits.mantissa == 0U)) {
        return "Negative Zero";
    }

    return "Normal";
}

bool float_equal(float a, float b, float epsilon) {
    if (fabsf(a - b) < epsilon) {
        return true;
    }

    return false;
}