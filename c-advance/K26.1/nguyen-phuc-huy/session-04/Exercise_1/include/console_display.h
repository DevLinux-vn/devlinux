#ifndef CONSOLE_DISPLAY_H
#define CONSOLE_DISPLAY_H

#include <stdint.h>

#include "i_display.h"

/**
 * @file console_display.h
 * @brief Public interface for the console display driver.
 */

/**
 * @brief Create or initialize a console display configuration.
 *
 * The returned pointer refers to an opaque configuration object whose
 * internal members are hidden from application code.
 *
 * @param[in] baud_rate Baud rate used by the simulated console display.
 *
 * @return Pointer to the console display configuration.
 */
display_config_t *console_config_create(uint32_t baud_rate);

/**
 * @brief Global console display interface instance.
 */
extern i_display_t console_display;

#endif /* CONSOLE_DISPLAY_H */