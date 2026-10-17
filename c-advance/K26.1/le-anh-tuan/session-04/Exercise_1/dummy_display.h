#ifndef DUMMY_DISPLAY_H
#define DUMMY_DISPLAY_H

#include <stdint.h>

#include "i_display.h"

/**
 * @brief Dummy display driver interface instance.
 */
extern i_display_t dummy_display;

/**
 * @brief Get the number of pixels sent to the dummy display.
 *
 * @return Number of calls made to the dummy draw-pixel function.
 */
uint32_t dummy_display_get_draw_count(void);

#endif /* DUMMY_DISPLAY_H */