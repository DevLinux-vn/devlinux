#include "uart_hal.h"

bool cmd_process_echo(void) {
    uint8_t buffer[64];
    uint32_t bytes_read = uart_hal_receive(buffer, sizeof(buffer));
    
    if (bytes_read > 0) {
        return uart_hal_transmit(buffer, bytes_read);
    }
    return false;
}