#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <stdbool.h>

/**
 * @brief Receives data from UART and echoes it back.
 *
 * Reads up to 64 bytes of data from the UART interface. If data is
 * received successfully, the received data is transmitted back through
 * the UART interface.
 *
 * @return true if data is received and transmitted successfully,
 *         false if no data is received or transmission fails.
 */
bool cmd_process_echo(void);

#endif /* COMMAND_HANDLER_H */