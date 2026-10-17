/**
 * @file    fake_uart.h
 * @brief   FFF (Fake Function Framework) fake declarations for the
 *          UART HAL, used to replace real hardware in unit tests.
 */
#ifndef FAKE_UART_H
#define FAKE_UART_H

#include "fff.h"
#include "uart_hal.h"

/** @brief Fake replacing uart_hal_receive() for testing. */
DECLARE_FAKE_VALUE_FUNC(uint32_t, uart_hal_receive, uint8_t *, uint32_t)

/** @brief Fake replacing uart_hal_transmit() for testing. */
DECLARE_FAKE_VALUE_FUNC(bool, uart_hal_transmit, const uint8_t *, uint32_t)

#endif /* FAKE_UART_H */