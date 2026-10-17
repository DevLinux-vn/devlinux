#ifndef CONSOLE_DISPLAY_H
#define CONSOLE_DISPLAY_H

#include <stdint.h>

#include "i_display.h"

/**
 * @brief Create the configuration for the console display.
 *
 * @param baud_rate Baud rate used by the simulated console display.
 *
 * @return Pointer to the opaque display configuration.
 */
display_config_t *console_config_create(uint32_t baud_rate);

/**
 * @brief Console display driver interface instance.
 */
extern i_display_t console_display;

#endif /* CONSOLE_DISPLAY_H */