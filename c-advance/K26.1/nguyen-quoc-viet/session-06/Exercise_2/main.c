#include <stdint.h>
#include <stdio.h>
#include "adc.h"

#define ADC_SAMPLE_RATE     (44100U)
#define ADC_CHANNEL         (2U)

#define APP_SUCCESS         (0)

int32_t main(void)
{
    (void)adc_init(ADC_SAMPLE_RATE);

    // adc_ctx.active_channel = 99;  // COMPILE ERROR

    (void)adc_init(ADC_SAMPLE_RATE);

    printf("Channel: %u\n", adc_get_channel());
    printf("Sample rate: %u Hz\n", adc_get_sample_rate());
    printf("Init status: %s\n", (true == adc_is_initialized()) ? "YES" : "NO");

    (void)adc_set_channel(ADC_CHANNEL);

    printf("Result: %u mV\n", adc_read());

    (void)adc_deinit();

    printf("Is initialized? %s\n", (true == adc_is_initialized()) ? "YES" : "NO");

    return APP_SUCCESS;
}