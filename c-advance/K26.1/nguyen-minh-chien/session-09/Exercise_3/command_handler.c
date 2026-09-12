/**
 * @file    command_handler.c
 * @brief   High-level command handler that echoes received UART data.
 */
#include "uart_hal.h"

/** @brief Size of the local receive buffer, in bytes. */
#define CMD_HANDLER_BUF_SIZE (64U)

/**
 * @brief   Read any pending UART data and echo it straight back.
 *
 * @details Reads up to @ref CMD_HANDLER_BUF_SIZE bytes from the UART
 *          receiver. If any bytes were received (CERT ERR33-C: the
 *          return value is checked before use), they are transmitted
 *          back unmodified. If no bytes are available, nothing is
 *          transmitted.
 *
 * @return  true if data was received and successfully transmitted
 *          back; false if no data was available or transmission
 *          failed.
 */
bool cmd_process_echo(void)
{
    uint8_t buffer[CMD_HANDLER_BUF_SIZE];
    uint32_t bytes_read = uart_hal_receive(buffer, sizeof(buffer));
    bool result = false;

    /* CERT ERR33-C: check the return value before using it. */
    if (bytes_read > 0U)
    {
        result = uart_hal_transmit(buffer, bytes_read);
    }

    return result;
}