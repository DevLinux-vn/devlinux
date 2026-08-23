#ifndef DUMMY_DISPLAY_H
#define DUMMY_DISPLAY_H

#include "i_display.h"

/**
 * @brief Dummy display driver implementation.
 *        This driver does not interact with any real hardware, but
 *        instead simulates a display for testing purposes.
 */
extern i_display_t dummy_display;

/**
 * @brief Get the number of pixels drawn since the last reset.
 * @return The number of pixels drawn.
 */
uint32_t dummy_display_get_draw_count(void);

#endif /*DUMMY_DISPLAY_H*/