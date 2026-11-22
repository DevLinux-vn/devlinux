/**
 * @file adc_driver.h
 * @brief Singleton ADC Driver Interface with Getter Functions.
 */

#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize the ADC peripheral with a specified sample rate.
 * @details Only succeeds if the ADC is not already initialized.
 * @param[in] sample_rate_hz Desired sampling rate in Hz.
 * @return true if initialization succeeded, false if already initialized.
 */
bool ADC_Init(uint32_t sample_rate_hz);

/**
 * @brief De-initialize the ADC peripheral and reset internal state.
 */
void ADC_DeInit(void);

/**
 * @brief Select the active ADC channel.
 * @param[in] channel ADC channel number to select.
 */
void ADC_SetChannel(uint32_t channel);

/**
 * @brief Read analog value from the currently active channel.
 * @return Sampled voltage in millivolts (mV), or 0 if not initialized.
 */
uint32_t ADC_Read(void);

/**
 * @brief Query whether the ADC peripheral is initialized.
 * @return true if initialized, false otherwise.
 */
bool ADC_IsInitialized(void);

/**
 * @brief Get the currently active ADC channel.
 * @return Active channel index.
 */
uint32_t ADC_GetChannel(void);

/**
 * @brief Get the configured sampling rate.
 * @return Sampling rate in Hz.
 */
uint32_t ADC_GetSampleRate(void);

#endif /* ADC_DRIVER_H */
