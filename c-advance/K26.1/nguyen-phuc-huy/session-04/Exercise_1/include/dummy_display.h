#ifndef DUMMY_DISPLAY_H
#define DUMMY_DISPLAY_H

#include <stdint.h>

#include "i_display.h"

/**
 * @file dummy_display.h
 * @brief Public interface for the dummy display driver.
 */

/**
 * @brief Global dummy display interface instance.
 */
extern i_display_t dummy_display;

/**
 * @brief Get the number of dummy pixel-drawing calls.
 *
 * @return Number of calls made to the dummy draw-pixel function.
 */
uint32_t dummy_display_get_draw_count(void);

/**
 * @brief Reset the dummy display draw counter.
 */
void dummy_display_reset_draw_count(void);

#endif /* DUMMY_DISPLAY_H */