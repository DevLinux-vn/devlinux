#ifndef CONSOLE_DISPLAY_H
#define CONSOLE_DISPLAY_H

#include "i_display.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Creates a configuration object for the console display driver.
 *
 * This factory function allocates and initializes a console display
 * configuration object with the specified baud rate. The returned pointer
 * is an opaque handle that hides the internal implementation details from
 * the application.
 *
 * @param[in] baud_rate Communication baud rate for the console display.
 *
 * @return Pointer to the created display configuration object.
 * @retval NULL Configuration creation failed.
 * @retval Non-NULL Successfully created configuration object.
 */
display_config_t *console_config_create(uint32_t baud_rate);

/**
 * @brief Console display driver instance.
 *
 * This global driver instance implements the @ref i_display_t interface.
 */
extern i_display_t console_display;

#endif /* CONSOLE_DISPLAY_H */