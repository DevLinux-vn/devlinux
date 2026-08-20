#include "adc.h"

#define APP_SUCCESS     (0U)
#define SAMPLE_RATE     (44100U)
#define CHANNEL         (2U)

int main()
{
    ADC_Init(SAMPLE_RATE);

    ADC_Init(SAMPLE_RATE);
    
    printf("Channel:     %u\n", ADC_GetChannel());
    printf("Sample rate: %u Hz\n", ADC_GetSampleRate());

    printf("Init status: %s\n", 
            ADC_IsInitialized() ? "YES" : "NO");

    ADC_SetChannel(CHANNEL);
    printf("Result: %u mV\n", ADC_Read());

	/* Compile error:
	* 's_adc_ctx' is not visible here because it is declared
	* as static inside adc.c (file scope).
	*/
	// s_adc_ctx.active_channel = 99U;

    ADC_DeInit();
    printf("Is initialized? %s\n", 
            ADC_IsInitialized() ? "YES" : "NO");

    return APP_SUCCESS;
}