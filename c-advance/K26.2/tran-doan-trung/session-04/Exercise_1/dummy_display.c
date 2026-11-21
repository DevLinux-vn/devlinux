/**
 * @file dummy_display.c
 * @brief Implementation of the dummy display driver interface.
 * @author Tran Doan Trung
 * @date 2026-11-21
 */
#include "dummy_display.h"

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Global counter tracking the total number of calls to dummy_display_draw_pixel.
 */
uint32_t dummy_draw_count = 0U;

/**
 * @brief Initialize the dummy display driver.
 * @param[in] p_config Pointer to the display configuration structure (unused).
 */
static void dummy_display_init(display_config_t *p_config) {
    if (p_config != NULL) {
        struct display_config_s *p_concrete = (struct display_config_s *)p_config;
        (void)p_concrete; 
    }
    (void)p_config; 
}

/**
 * @brief Dummy draw pixel function that increments the global call counter without rendering.
 * @param[in] x X coordinate of the pixel (unused).
 * @param[in] y Y coordinate of the pixel (unused).
 * @param[in] color Color value of the pixel (unused).
 */
static void dummy_display_draw_pixel(uint16_t x, uint16_t y, uint8_t color) {
    (void)x; 
    (void)y; 
    (void)color; 
    dummy_draw_count++;
}

/**
 * @brief Instance of the dummy display interface implementing i_display_t operations.
 */
i_display_t dummy_display = {
    .init = dummy_display_init,
    .draw_pixel = dummy_display_draw_pixel
};
