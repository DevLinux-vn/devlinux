/**
 * @file adc_driver.c
 * @brief Implementation of Singleton ADC Driver with Accessor Functions.
 */

#include "adc_driver.h"
#include <stdio.h>

/**
 * @brief Private ADC context structure (Singleton pattern).
 * @details Hidden inside this translation unit, not exposed in any header file.
 */
static struct {
    bool     is_initialized; /**< Guards against double-initialization. */
    uint32_t active_channel; /**< Currently selected ADC channel.       */
    uint32_t sample_rate_hz; /**< Configured sampling rate in Hz.       */
} s_adc_ctx = { false, 0U, 0U };

/**
 * @brief Initialize the ADC peripheral with a specified sample rate.
 * @details Validates that the ADC is not already initialized. Sets default channel 0
 *          and stores sampling rate. Prevents multiple initializations.
 * @param[in] sample_rate_hz Desired sampling rate in Hz (e.g. 44100).
 * @return true if initialization succeeded.
 * @return false if ADC was already initialized.
 */
bool ADC_Init(uint32_t sample_rate_hz)
{
    if (s_adc_ctx.is_initialized) {
        printf("[ADC] Error: Already initialized! Call ADC_DeInit() first.\n");
        return false;
    }

    s_adc_ctx.is_initialized = true;
    s_adc_ctx.active_channel = 0U;
    s_adc_ctx.sample_rate_hz = sample_rate_hz;
    printf("[ADC] Initialized at %u Hz on channel 0.\n", sample_rate_hz);
    return true;
}

/**
 * @brief De-initialize the ADC peripheral and reset internal state.
 * @details Resets active channel, sampling rate, and initialized flag to defaults.
 */
void ADC_DeInit(void)
{
    s_adc_ctx.is_initialized = false;
    s_adc_ctx.active_channel = 0U;
    s_adc_ctx.sample_rate_hz = 0U;
    printf("[ADC] De-initialized.\n");
}

/**
 * @brief Select the active ADC channel.
 * @details Checks if ADC is initialized before updating active channel.
 * @param[in] channel ADC channel number to select (e.g. 0, 1, 2).
 */
void ADC_SetChannel(uint32_t channel)
{
    if (!s_adc_ctx.is_initialized) {
        printf("[ADC] Error: Not initialized! Call ADC_Init() first.\n");
        return;
    }

    s_adc_ctx.active_channel = channel;
    printf("[ADC] Channel set to %u.\n", channel);
}

/**
 * @brief Read analog conversion result from the currently active channel.
 * @details Checks initialization guard, performs simulated ADC read,
 *          and outputs conversion voltage.
 * @return Sampled voltage in millivolts (mV), or 0 if uninitialized.
 */
uint32_t ADC_Read(void)
{
    if (!s_adc_ctx.is_initialized) {
        printf("[ADC] Error: Not initialized! Call ADC_Init() first.\n");
        return 0U;
    }

    /* Simulated conversion result: 300 mV */
    const uint32_t simulated_voltage_mv = 300U;
    printf("[ADC] Read ch%u -> %u mV\n", s_adc_ctx.active_channel, simulated_voltage_mv);
    return simulated_voltage_mv;
}

/**
 * @brief Query whether the ADC peripheral is currently initialized.
 * @details Getter function providing read-only access to initialization status.
 * @return true if initialized and ready for conversion.
 * @return false if uninitialized or de-initialized.
 */
bool ADC_IsInitialized(void)
{
    return s_adc_ctx.is_initialized;
}

/**
 * @brief Get the currently active ADC channel index.
 * @details Getter function providing read-only access to active channel setting.
 * @return Active channel index (uint32_t).
 */
uint32_t ADC_GetChannel(void)
{
    return s_adc_ctx.active_channel;
}

/**
 * @brief Get the configured sampling rate.
 * @details Getter function providing read-only access to sampling rate.
 * @return Configured sampling rate in Hz (uint32_t).
 */
uint32_t ADC_GetSampleRate(void)
{
    return s_adc_ctx.sample_rate_hz;
}
