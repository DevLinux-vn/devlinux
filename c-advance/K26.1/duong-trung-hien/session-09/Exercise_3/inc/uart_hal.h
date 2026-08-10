#ifndef UART_HAL_H
#define UART_HAL_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Receives data from the UART interface.
 *
 * Reads up to @p max_len bytes from the UART interface and stores the
 * received data in the provided buffer.
 *
 * @param[out] buf     Buffer used to store the received data.
 * @param[in]  max_len Maximum number of bytes that can be stored in @p buf.
 *
 * @return The number of bytes actually received.
 */
uint32_t uart_hal_receive(uint8_t *buf, uint32_t max_len);

/**
 * @brief Transmits data through the UART interface.
 *
 * Sends the specified number of bytes from the provided buffer through
 * the UART interface.
 *
 * @param[in] buf Buffer containing the data to transmit.
 * @param[in] len Number of bytes to transmit.
 *
 * @return true if the data is transmitted successfully,
 *         false otherwise.
 */
bool uart_hal_transmit(const uint8_t *buf, uint32_t len);

#endif /* UART_HAL_H */