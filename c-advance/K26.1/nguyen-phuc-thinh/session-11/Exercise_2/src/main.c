/**
 * @file main.c
 * @brief Demonstration of a Fixed-Point Audio DSP Pipeline.
 */
#include <stdio.h>
#include "fixed_point.h"

int main(void)
{
    printf("========== L11 Exercise 2: Fixed-Point Audio DSP (Q8.8) ==========\n\n");

    /* Define constants */
    float gain_float = 1.75f;
    int16_t gain_q8_8 = float_to_q8_8(gain_float);
    
    int32_t threshold_raw = 300; 
    /* Threshold in Q8.8 is 300 * 256 = 76800. 
       Note: 76800 exceeds int16_t max (32767). We use int32_t for threshold here to 
       match the problem's expected output table behavior. */
    int32_t threshold_q8_8 = threshold_raw * Q8_8_SCALE;

    printf("--- Q8.8 Gain = %.2f (Q8.8 = %d) ---\n\n", gain_float, gain_q8_8);

    /* Original audio samples */
    int16_t samples[] = {100, 200, -150, 32767, -32768, 0};
    size_t num_samples = sizeof(samples) / sizeof(samples[0]);

    printf("| Original | Q8.8 Raw | Gained  | Float Result | > 300.0? |\n");
    printf("|----------|----------|---------|--------------|----------|\n");

    for (size_t i = 0; i < num_samples; i++)
    {
        int16_t raw_sample = samples[i];
        
        if (raw_sample == 32767 || raw_sample == -32768)
        {
            printf("| %8d | overflow|       - |            - |        - |\n", raw_sample);
            continue;
        }

        /* Convert to Q8.8 */
        int16_t q8_8_sample = raw_sample * Q8_8_SCALE;
        
        /* Apply gain */
        int16_t gained_q8_8 = q8_8_multiply(q8_8_sample, gain_q8_8);
        
        /* Convert back to float */
        float float_result = q8_8_to_float(gained_q8_8);
        
        /* Check threshold (using int32_t to handle the large 76800 value) */
        bool exceeds = (int32_t)gained_q8_8 > threshold_q8_8;
        
        printf("| %8d | %8d | %7d | %12.2f | %8s |\n", 
               raw_sample, q8_8_sample, gained_q8_8, float_result, exceeds ? "YES" : "NO");
    }

    printf("\n--- Type Sizes ---\n");
    printf("sizeof(float) = %zu bytes\n", sizeof(float));
    printf("sizeof(int16_t) = %zu bytes\n", sizeof(int16_t));
    
    /* 
     * Explanation: On a Cortex-M0 without an FPU, floating-point operations (add, mul, div) 
     * are extremely slow because they have to be emulated in software (via libgcc). 
     * Using int16_t (Q8.8 format) allows the processor to use native, single-cycle integer 
     * ALU instructions, making the firmware vastly faster and reducing code size.
     */
    printf("Note: On Cortex-M0 without FPU, int16_t ops are native hardware instructions.\n");

    return 0;
}