#ifndef CONSOLE_DISPLAY_H
#define CONSOLE_DISPLAY_H

#include "i_display.h"

/**
 * @brief Global console display instance, wiring interface function pointers
 *        to the console implementations.
 */
extern i_display_t console_display;

/**
 * @brief Factory function that creates and initializes a console display
 *        configuration.
 * @param baud_rate The baud rate to store in the configuration.
 * @return Opaque pointer to the newly created configuration.
 */
display_config_t *console_config_create(uint32_t baud_rate);

#endif /* CONSOLE_DISPLAY_H */