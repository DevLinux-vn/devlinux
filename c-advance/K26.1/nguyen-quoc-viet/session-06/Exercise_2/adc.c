#include "adc.h"

/**
 * @brief Stores the internal ADC driver state.
 *
 * This structure is private to the ADC module and maintains the
 * initialization status and current ADC configuration.
 */
typedef struct st_adc_info
{
    bool     is_initialized; /**< Guards against double-initialization. */
    uint32_t active_channel; /**< Currently selected ADC channel.       */
    uint32_t sample_rate_hz; /**< Configured sampling rate in Hz.       */
} st_adc_info_t;

/**
 * @brief Internal ADC driver context.
 *
 * This static instance represents the single ADC driver state and is
 * inaccessible outside this source file.
 */
static st_adc_info_t adc_ctx =
{
    .is_initialized = false,
    .active_channel = 0U,
    .sample_rate_hz = 0U
};

/**
 * @brief Simulated ADC conversion value.
 *
 * This value represents the sample returned by adc_read() when the ADC
 * module has been initialized.
 */
// NOLINTNEXTLINE(readability-magic-numbers)
static uint32_t g_adc_value = 300U;

/**
 * @brief Initialize the ADC module.
 */
bool adc_init(const uint32_t sample_rate_hz)
{
    bool result = true;

    if (true == adc_ctx.is_initialized)
    {
        printf("[ADC] Error: Already initialized! Call adc_deinit() first.\n");
        result = false;
    }
    else if (0U == sample_rate_hz)
    {
        printf("[ERROR] %s: Invalid sample rate!\n", __func__);
        result = false;
    }
    else
    {
        adc_ctx.is_initialized = true;
        adc_ctx.sample_rate_hz = sample_rate_hz;
        printf("[ADC] Initialized at %u Hz on channel %u.\n", sample_rate_hz, adc_ctx.active_channel);
    }

    return result;
}

/**
 * @brief Deinitialize the ADC module.
 */
bool adc_deinit(void)
{
    bool result = true;

    if (false == adc_ctx.is_initialized)
    {
        printf("[ERROR] %s: ADC is not yet initialized!\n", __func__);
        result = false;
    }
    else
    {
        adc_ctx.is_initialized = false;
        adc_ctx.active_channel = 0U;
        adc_ctx.sample_rate_hz = 0U;
    }

    return result;
}

/**
 * @brief Select the active ADC channel.
 */
bool adc_set_channel(const uint32_t channel)
{
    bool result = true;

    if (false == adc_ctx.is_initialized)
    {
        printf("[ERROR] %s: ADC is not yet initialized!\n", __func__);
        result = false;
    }
    else
    {
        printf("[ADC] Channel set to %u.\n", channel);
        adc_ctx.active_channel = channel;
    }

    return result;
}

/**
 * @brief Read the current ADC conversion value.
 */
uint32_t adc_read(void)
{
    uint32_t adc_output = 0U;

    if (true == adc_ctx.is_initialized)
    {
        adc_output = g_adc_value;
        printf("[ADC] Read ch%u -> %u mV\n", adc_get_channel(), adc_output);
    }

    return adc_output;
}

/**
 * @brief Query whether the ADC module is initialized.
 */
bool adc_is_initialized(void)
{
    return adc_ctx.is_initialized;
}

/**
 * @brief Get the currently selected ADC channel.
 */
uint32_t adc_get_channel(void)
{
    return adc_ctx.active_channel;
}

/**
 * @brief Get the configured ADC sampling rate.
 */
uint32_t adc_get_sample_rate(void)
{
    return adc_ctx.sample_rate_hz;
}