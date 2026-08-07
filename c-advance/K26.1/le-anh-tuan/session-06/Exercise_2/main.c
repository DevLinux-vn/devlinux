#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Private ADC driver context.
 *
 * Stores the complete state of the singleton ADC driver.
 * This structure is private to this translation unit and must
 * never be exposed directly to application code.
 */
typedef struct
{
    bool is_initialized;       /**< True when the ADC has been initialized. */
    uint32_t active_channel;   /**< Currently selected ADC channel. */
    uint32_t sample_rate_hz;   /**< Configured ADC sample rate in Hz. */
} adc_context_t;

/**
 * @brief Singleton ADC driver instance.
 *
 * Only one ADC context exists for the entire program.
 * File-scope static linkage prevents other translation units
 * from accessing this object directly.
 */
static adc_context_t s_adc_ctx = {false, 0U, 0U};

/**
 * @brief Initialize the ADC driver.
 *
 * Initializes the singleton ADC instance with the requested
 * sampling frequency. The initial ADC channel is channel 0.
 *
 * A second initialization attempt is rejected until ADC_DeInit()
 * has been called.
 *
 * @param sample_rate_hz Desired ADC sampling frequency in Hz.
 *
 * @return true if initialization succeeds.
 * @return false if the ADC has already been initialized.
 */
bool ADC_Init(uint32_t sample_rate_hz)
{
    bool result = false;

    if (s_adc_ctx.is_initialized)
    {
        (void)printf(
            "[ADC] Error: Already initialized! Call ADC_DeInit() first.\n");
    }
    else
    {
        s_adc_ctx.is_initialized = true;
        s_adc_ctx.active_channel = 0U;
        s_adc_ctx.sample_rate_hz = sample_rate_hz;

        (void)printf(
            "[ADC] Initialized at %lu Hz on channel %lu.\n",
            (unsigned long)s_adc_ctx.sample_rate_hz,
            (unsigned long)s_adc_ctx.active_channel);

        result = true;
    }

    return result;
}

/**
 * @brief De-initialize the ADC driver.
 *
 * Resets all internal ADC state to its default values.
 */
void ADC_DeInit(void)
{
    s_adc_ctx.is_initialized = false;
    s_adc_ctx.active_channel = 0U;
    s_adc_ctx.sample_rate_hz = 0U;

    (void)printf("[ADC] De-initialized.\n");
}

/**
 * @brief Select the active ADC channel.
 *
 * The operation is rejected if the ADC has not first been
 * initialized using ADC_Init().
 *
 * @param channel ADC channel number to select.
 */
void ADC_SetChannel(uint32_t channel)
{
    if (!s_adc_ctx.is_initialized)
    {
        (void)printf(
            "[ADC] Error: ADC is not initialized.\n");
    }
    else
    {
        s_adc_ctx.active_channel = channel;

        (void)printf(
            "[ADC] Channel set to %lu.\n",
            (unsigned long)s_adc_ctx.active_channel);
    }
}

/**
 * @brief Read a mock ADC conversion result.
 *
 * This exercise does not use real ADC hardware. A deterministic
 * mock value is generated from the selected channel:
 *
 * result = (channel + 1) * 100 mV
 *
 * Therefore, channel 2 produces 300 mV.
 *
 * @return Mock ADC measurement in millivolts.
 * @return 0U if the ADC has not been initialized.
 */
uint32_t ADC_Read(void)
{
    uint32_t result_mv = 0U;

    if (!s_adc_ctx.is_initialized)
    {
        (void)printf(
            "[ADC] Error: ADC is not initialized.\n");
    }
    else
    {
        result_mv = (s_adc_ctx.active_channel + 1U) * 100U;

        (void)printf(
            "[ADC] Read ch%lu -> %lu mV\n",
            (unsigned long)s_adc_ctx.active_channel,
            (unsigned long)result_mv);
    }

    return result_mv;
}

/**
 * @brief Determine whether the ADC is initialized.
 *
 * This getter provides controlled read-only access to the
 * initialization state without exposing the private ADC context.
 *
 * @return true if the ADC is initialized.
 * @return false otherwise.
 */
bool ADC_IsInitialized(void)
{
    return s_adc_ctx.is_initialized;
}

/**
 * @brief Get the currently selected ADC channel.
 *
 * This getter provides controlled read-only access to the
 * active channel.
 *
 * @return Current ADC channel number.
 */
uint32_t ADC_GetChannel(void)
{
    return s_adc_ctx.active_channel;
}

/**
 * @brief Get the configured ADC sampling frequency.
 *
 * This getter provides controlled read-only access to the
 * configured sample rate.
 *
 * @return ADC sample rate in Hz.
 */
uint32_t ADC_GetSampleRate(void)
{
    return s_adc_ctx.sample_rate_hz;
}

/**
 * @brief Program entry point used to demonstrate the ADC singleton.
 *
 * Demonstrates initialization, the double-initialization guard,
 * getter functions, channel selection, ADC reading, and
 * de-initialization.
 *
 * @return 0 on successful completion.
 */
int main(void)
{
    uint32_t adc_result_mv;

    (void)ADC_Init(44100U);

    /* Verify that a second initialization is rejected. */
    (void)ADC_Init(48000U);

    (void)printf(
        "Channel:     %lu\n",
        (unsigned long)ADC_GetChannel());

    (void)printf(
        "Sample rate: %lu Hz\n",
        (unsigned long)ADC_GetSampleRate());

    (void)printf(
        "Init status: %s\n",
        ADC_IsInitialized() ? "YES" : "NO");

    ADC_SetChannel(2U);

    adc_result_mv = ADC_Read();

    (void)printf(
        "Result: %lu mV\n",
        (unsigned long)adc_result_mv);

    /*
     * Direct access must not be used by another module:
     *
     * s_adc_ctx.active_channel = 99U;
     *
     * COMPILE ERROR from another translation unit because
     * s_adc_ctx has static linkage and is private to this .c file.
     * Other modules must use ADC_SetChannel() and the getter API.
     */

    ADC_DeInit();

    (void)printf(
        "Is initialized? %s\n",
        ADC_IsInitialized() ? "YES" : "NO");

    return 0;
}