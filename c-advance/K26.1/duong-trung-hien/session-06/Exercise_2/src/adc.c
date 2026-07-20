#include "adc.h"

static struct {
    bool        is_initialized;     /**< Guards against double-initialization. */
    uint32_t    active_channel;     /**< Currently selected ADC channel.       */
    uint32_t    sample_rate_hz;     /**< Configured sampling rate in Hz.       */
} s_adc_ctx = {false, 0U, 0U};

bool ADC_Init(uint32_t sample_rate_hz)
{
    if (s_adc_ctx.is_initialized)
    {
        printf("[ADC] Error: Already initialized! Call ADC_DeInit() first.\n");
        return false;
    }

    s_adc_ctx.is_initialized = true;
    s_adc_ctx.sample_rate_hz = sample_rate_hz;

    printf("[ADC] Initialized at %u Hz on channel %u.\n", 
            s_adc_ctx.sample_rate_hz, 
            s_adc_ctx.active_channel);
    
    return true;
}

void ADC_DeInit(void)
{
    printf("[ADC] De-initialized.\n");
    s_adc_ctx.is_initialized = false;
    s_adc_ctx.active_channel = 0U;
    s_adc_ctx.sample_rate_hz = 0U;
}

void ADC_SetChannel(uint32_t channel)
{
    if (!s_adc_ctx.is_initialized )
    {
        return;
    }

    s_adc_ctx.active_channel = channel;
    printf("[ADC] Channel set to %u.\n", s_adc_ctx.active_channel);
}

uint32_t ADC_Read(void)
{
    if (!s_adc_ctx.is_initialized )
    {
        return ADC_INVALID_VALUE;
    }

    uint32_t value;
    value = (s_adc_ctx.active_channel + 1U) * 100U;
    printf("[ADC] Read channel %u -> %u mV\n", 
            s_adc_ctx.active_channel, 
            value);

    return value;
}

bool ADC_IsInitialized(void)
{
    return s_adc_ctx.is_initialized;
}

uint32_t ADC_GetChannel(void)
{
    return s_adc_ctx.active_channel;
}

uint32_t ADC_GetSampleRate(void)
{
    return s_adc_ctx.sample_rate_hz;
}