#ifndef DUMMY_DISPLAY_H
#define DUMMY_DISPLAY_H

#include "i_display.h"
#include <stdint.h>

/**
 * @brief Dummy display driver instance.
 *
 * This global driver instance implements the @ref i_display_t interface.
 * It provides a mock display backend for testing application logic
 * without requiring actual display hardware.
 */
extern i_display_t dummy_display;

/**
 * @brief Returns the number of pixel draw operations.
 *
 * Retrieves the total number of times the dummy display driver's
 * draw_pixel() function has been called since program startup or
 * since the counter was last reset.
 *
 * @return Total number of draw operations performed by the dummy display.
 */
uint32_t dummy_get_draw_count(void);

#endif /* DUMMY_DISPLAY_H */