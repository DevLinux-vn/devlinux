#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define ADC_INVALID_VALUE UINT32_MAX

/**
 * @brief Initialize the ADC driver.
 *
 * Initializes the ADC with the specified sampling rate. This function
 * implements the Singleton pattern and prevents multiple initializations.
 *
 * @param sample_rate_hz ADC sampling rate in Hertz.
 *
 * @return true  ADC initialized successfully.
 * @return false ADC is already initialized.
 */
bool ADC_Init(uint32_t sample_rate_hz);

/**
 * @brief De-initialize the ADC driver.
 *
 * Resets the internal ADC context to its default state and releases
 * the ADC instance for future initialization.
 */
void ADC_DeInit(void);

/**
 * @brief Select the active ADC channel.
 *
 * Sets the ADC channel used for subsequent read operations.
 * If the ADC has not been initialized, the request is ignored.
 *
 * @param channel ADC channel number to select.
 */
void ADC_SetChannel(uint32_t channel);

/**
 * @brief Read a value from the active ADC channel.
 *
 * Returns a simulated ADC conversion result from the currently selected
 * channel. If the ADC has not been initialized, an invalid value is returned.
 *
 * @return Simulated ADC reading in millivolts, or
 *         ADC_INVALID_VALUE if the ADC is not initialized.
 */
uint32_t ADC_Read(void);

/**
 * @brief Check whether the ADC driver is initialized.
 *
 * @return true  ADC has been initialized.
 * @return false ADC has not been initialized.
 */
bool ADC_IsInitialized(void);

/**
 * @brief Get the currently selected ADC channel.
 *
 * @return Current ADC channel number.
 */
uint32_t ADC_GetChannel(void);

/**
 * @brief Get the configured ADC sampling rate.
 *
 * @return ADC sampling rate in Hertz.
 */
uint32_t ADC_GetSampleRate(void);

#endif /* ADC_H */