/**
 * @file    main.c
 * @brief   Single-instance ADC driver demonstrating the Singleton pattern.
 * @details Internal state is hidden as a static struct; access is only
 *          possible through the public API and getter functions.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

/**
 * @brief Internal ADC driver state (Singleton instance).
 *
 * This struct is declared static at file scope so it has internal
 * linkage — it cannot be referenced from any other translation unit,
 * even with an extern declaration, because no such symbol is exported.
 */
static struct
{
    bool     is_initialized;   /**< true if ADC_Init() has been called successfully. */
    uint32_t active_channel;   /**< Currently selected ADC channel.                  */
    uint32_t sample_rate_hz;   /**< Configured sampling rate, in Hz.                 */
} s_adc_ctx = { false, 0U, 0U };

/**
 * @brief  Initializes the singleton ADC instance.
 * @param  sample_rate_hz Desired sampling rate in Hz (must be > 0).
 * @return true if initialization succeeded; false if already
 *         initialized (guard against double-init).
 * @note   Follows the single-instance guard pattern — a second call
 *         before ADC_DeInit() is rejected.
 */
bool ADC_Init(uint32_t sample_rate_hz)
{
    bool result;

    if (s_adc_ctx.is_initialized == true)
    {
        printf("[ADC] Error: Already initialized! Call ADC_DeInit() first.\n");
        result = false;
    }
    else
    {
        /* TODO: gán is_initialized = true, sample_rate_hz, active_channel = 0 */
        s_adc_ctx.is_initialized = true;
        s_adc_ctx.sample_rate_hz = sample_rate_hz;
        s_adc_ctx.active_channel = 0;
        printf("[ADC] Initialized at %" PRIu32 " Hz on channel %" PRIu32 ".\n",s_adc_ctx.sample_rate_hz, s_adc_ctx.active_channel);
        result = true;
    }

    return result;   
}

/**
 * @brief  Selects the active ADC channel.
 * @param  channel Channel index to select.
 * @note   No effect if the driver has not been initialized.
 */
void ADC_SetChannel(uint32_t channel)
{
    if (!s_adc_ctx.is_initialized) { 
        printf("[ADC] Error: Already initialized! Call ADC_DeInit() first.\n"); 
        return; 
    }
    else 
    { 
        s_adc_ctx.active_channel = channel; 
        printf("Channel set to %u", s_adc_ctx.active_channel);
    }
}

/**
 * @brief  Reads the current ADC value on the active channel.
 * @return Simulated ADC reading in millivolts, or 0 if not initialized.
 */
uint32_t ADC_Read(void)
{
    uint32_t reading;

    if (s_adc_ctx.is_initialized == false)
    {
        printf("[ADC] Error: Already initialized! Call ADC_DeInit() first.\n");
        reading = 0U;
    }
    else
    {
        reading = 300U;
        printf("[ADC] Read ch%u -> %u mV\n",
           (unsigned int)s_adc_ctx.active_channel,
           (unsigned int)reading);
    }

    return reading;
}

/**
 * @brief De-initializes the ADC, resetting all internal state to defaults.
 */
void ADC_DeInit(void)
{
    s_adc_ctx.is_initialized = false;
    s_adc_ctx.active_channel = 0U ;
    s_adc_ctx.sample_rate_hz = 0U ;
    printf("[ADC] De-initialized.\n");
}

/**
 * @brief  Reports whether the ADC has been initialized.
 * @return true if initialized, false otherwise.
 */
bool ADC_IsInitialized(void)
{
    return s_adc_ctx.is_initialized;
}

/**
 * @brief  Retrieves the currently selected ADC channel.
 * @return Active channel index.
 */
uint32_t ADC_GetChannel(void)
{
    return s_adc_ctx.active_channel;
}

/**
 * @brief  Retrieves the configured sample rate.
 * @return Sample rate in Hz.
 */
uint32_t ADC_GetSampleRate(void)
{
    return s_adc_ctx.sample_rate_hz;
}

/**
 * @brief  Entry point demonstrating the Singleton ADC driver.
 * @return 0 on success.
 */
int main(void)
{
    uint32_t reading;

    /* Step 1: normal initialization */
    (void)ADC_Init(44100U);

    /* Step 2: attempt double-init -> guard must fire, prints error */
    (void)ADC_Init(48000U);

    /* Step 3: read state via getters only (NOT via s_adc_ctx directly) */
    printf("Channel:     %u\n", (unsigned int)ADC_GetChannel());
    printf("Sample rate: %u Hz\n", (unsigned int)ADC_GetSampleRate());
    printf("Init status: %s\n", ADC_IsInitialized() ? "YES" : "NO");

    /* Direct access to the internal struct is impossible from here because
     * s_adc_ctx has internal linkage (static, file-scope) -- it is only
     * visible inside this translation unit's declaration point, and even
     * within the same file, encapsulation is enforced by convention:
     * external modules have no declaration of s_adc_ctx at all, so the
     * compiler reports it as an undeclared identifier. */
    // s_adc_ctx.active_channel = 99;
    // COMPILE ERROR: 's_adc_ctx' is static at file scope in this
    // translation unit; other .c files have no visibility of this symbol,
    // even with an extern declaration, since none is exported.

    /* Step 4: set channel and read */
    ADC_SetChannel(2U);
    reading = ADC_Read();
    printf("Result: %u mV\n", (unsigned int)reading);

    /* Step 5: de-init and verify clean state */
    ADC_DeInit();
    printf("Is initialized? %s\n", ADC_IsInitialized() ? "YES" : "NO");

    return 0;
}