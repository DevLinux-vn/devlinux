/**
 * @file dummy_display.h
 * @brief Dummy display driver interface used for testing and call metrics.
 * @author Tran Doan Trung
 * @date 2026-11-21
 */
#ifndef I_DUMMY_DISPLAY_H
#define I_DUMMY_DISPLAY_H

#include "i_display.h"

/**
 * @brief Dummy display driver interface instance.
 */
extern i_display_t dummy_display;

/**
 * @brief Global counter tracking the total number of calls to dummy_display_draw_pixel.
 */
extern uint32_t dummy_draw_count;

#endif /* I_DUMMY_DISPLAY_H */
