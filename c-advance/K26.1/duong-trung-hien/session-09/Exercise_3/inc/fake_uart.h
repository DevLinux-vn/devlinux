#ifndef FAKE_UART_H
#define FAKE_UART_H

#include "fff.h"
#include "uart_hal.h"

/**
 * @brief Fake implementation of the UART receive function.
 *
 * Used in unit tests to simulate receiving data from the UART interface
 * without requiring actual UART hardware.
 *
 * @return The number of bytes received.
 */
DECLARE_FAKE_VALUE_FUNC(uint32_t, uart_hal_receive, uint8_t*, uint32_t);

/**
 * @brief Fake implementation of the UART transmit function.
 *
 * Used in unit tests to simulate transmitting data through the UART
 * interface without requiring actual UART hardware.
 *
 * @return true if the simulated transmission is successful,
 *         false otherwise.
 */
DECLARE_FAKE_VALUE_FUNC(bool, uart_hal_transmit, const uint8_t*, uint32_t);

#endif /* FAKE_UART_H */