/**
 * @file main.c
 * @brief Application demonstrating Singleton ADC Driver with Getter Functions.
 */

#include "adc_driver.h"
#include <stdio.h>

/**
 * @brief Main entry point verifying ADC driver operations.
 * @return 0 on success.
 */
int main(void)
{
    /* 1. Initial initialization */
    (void)ADC_Init(44100U);

    /* 2. Attempt double-initialization: verify guard fires */
    (void)ADC_Init(44100U);

    /* 3. Query state using getter functions */
    printf("Channel:     %u\n", ADC_GetChannel());
    printf("Sample rate: %u Hz\n", ADC_GetSampleRate());
    printf("Init status: %s\n", ADC_IsInitialized() ? "YES" : "NO");

    /* 4. Configure channel and read analog value */
    ADC_SetChannel(2U);
    uint32_t result = ADC_Read();
    printf("Result: %u mV\n", result);

    /* 5. De-initialize and verify clean state */
    ADC_DeInit();
    printf("Is initialized? %s\n", ADC_IsInitialized() ? "YES" : "NO");

    /*
     * Direct access demonstration requirement:
     * s_adc_ctx.active_channel = 99U;
     * COMPILE ERROR: 's_adc_ctx' undeclared here (static struct is encapsulated
     * inside adc_driver.c and completely inaccessible from main.c).
     */

    return 0;
}
