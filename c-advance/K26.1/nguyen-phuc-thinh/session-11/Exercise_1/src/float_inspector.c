/**
 * @file float_inspector.c
 * @brief Implementation of the float inspector module.
 */
#include "float_inspector.h"
#include <stdio.h>
#include <math.h>

void inspect_float(float value)
{
    float_inspector_t inspector;
    inspector.f = value;
    
    /* IEEE-754 single precision exponent bias is 127 */
    int32_t actual_exp = (int32_t)inspector.bits.exponent - 127;
    
    printf("Value: %f | Sign: %u | Exp: %u (Actual: %d) | Mantissa: 0x%06X\n",
           (double)value, 
           inspector.bits.sign, 
           inspector.bits.exponent, 
           actual_exp, 
           inspector.bits.mantissa);
}

const char* classify_float(float value)
{
    float_inspector_t inspector;
    inspector.f = value;

    if (isinf(value))
    {
        if (inspector.bits.sign == 1U)
        {
            return "-Infinity";
        }
        return "+Infinity";
    }
    
    if (isnan(value))
    {
        return "NaN";
    }
    
    if (value == 0.0f)
    {
        if (inspector.bits.sign == 1U)
        {
            return "Negative Zero";
        }
    }
    
    return "Normal";
}

bool float_equal(float a, float b, float epsilon)
{
    /* Use fabsf for single-precision absolute value */
    return fabsf(a - b) < epsilon;
}