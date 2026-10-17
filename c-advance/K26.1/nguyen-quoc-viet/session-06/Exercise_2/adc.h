#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Initialize the ADC module.
 *
 * Initializes the internal ADC context and configures the requested
 * sampling rate. Calling this function after the ADC has already been
 * initialized is rejected.
 *
 * @param[in] sample_rate_hz Sampling rate in hertz. Must be greater than zero.
 *
 * @return Initialization result.
 * @retval true  ADC initialized successfully.
 * @retval false ADC was already initialized or the sampling rate was invalid.
 */
bool adc_init(const uint32_t sample_rate_hz);

/**
 * @brief Deinitialize the ADC module.
 *
 * Clears the current ADC configuration and returns the internal context
 * to its default state.
 *
 * @return Deinitialization result.
 * @retval true  ADC deinitialized successfully.
 * @retval false ADC had not been initialized.
 */
bool adc_deinit(void);

/**
 * @brief Select the active ADC channel.
 *
 * Updates the ADC channel used by subsequent adc_read() operations.
 * The ADC module must be initialized before this function is called.
 *
 * @param[in] channel ADC channel to select.
 *
 * @return Channel selection result.
 * @retval true  Channel selected successfully.
 * @retval false ADC has not been initialized.
 */
bool adc_set_channel(const uint32_t channel);

/**
 * @brief Read the current ADC conversion value.
 *
 * Returns the simulated ADC sample when the ADC module is initialized.
 *
 * @return Current ADC conversion value.
 * @retval 0U ADC has not been initialized.
 *
 * @note A return value of 0U may also represent a valid ADC sample.
 *       Call adc_is_initialized() when the initialization state must be
 *       distinguished from a valid zero conversion result.
 */
uint32_t adc_read(void);

/**
 * @brief Query whether the ADC module is initialized.
 *
 * @return Current ADC initialization state.
 * @retval true  ADC is initialized.
 * @retval false ADC is not initialized.
 */
bool adc_is_initialized(void);

/**
 * @brief Get the currently selected ADC channel.
 *
 * @return Currently selected ADC channel.
 *
 * @note The returned value is 0U after initialization until another
 *       channel is selected, and after the ADC is deinitialized.
 */
uint32_t adc_get_channel(void);

/**
 * @brief Get the configured ADC sampling rate.
 *
 * @return Configured sampling rate in hertz.
 * @retval 0U ADC is not initialized or has been deinitialized.
 */
uint32_t adc_get_sample_rate(void);

#endif