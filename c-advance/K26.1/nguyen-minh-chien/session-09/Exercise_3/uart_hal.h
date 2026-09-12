/**
 * @file    uart_hal.h
 * @brief   Hardware Abstraction Layer (HAL) interface for UART.
 *
 * @details This header only declares the UART HAL functions. In this
 *          test project, no real implementation of these functions is
 *          linked - a fake implementation (see fake_uart.h) is used
 *          instead, so tests can run without real UART hardware.
 */
#ifndef UART_HAL_H
#define UART_HAL_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief   Receive bytes from the UART hardware.
 *
 * @param[out] buf     Destination buffer for received bytes.
 * @param[in]  max_len Maximum number of bytes @p buf can hold.
 *
 * @return  The number of bytes actually received (0 if none available).
 */
uint32_t uart_hal_receive(uint8_t *buf, uint32_t max_len);

/**
 * @brief   Transmit bytes over the UART hardware.
 *
 * @param[in] buf Buffer containing the bytes to send.
 * @param[in] len Number of bytes to send from @p buf.
 *
 * @return  true if the transmission succeeded, false otherwise.
 */
bool uart_hal_transmit(const uint8_t *buf, uint32_t len);

#endif /* UART_HAL_H */