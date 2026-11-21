/**
 * @file console_display.c
 * @brief Implementation of the console display driver functionality.
 * @author Tran Doan Trung
 * @date 2026-11-21
 */
#include "console_display.h"

#include <stdio.h>
#include <stdint.h>

/**
 * @brief Concrete implementation of the display configuration structure for console.
 */
struct display_config_s {
    uint32_t baud_rate; /**< Baud rate for console serial communication. */
};

/**
 * @brief Static configuration storage for the console display driver.
 */
static display_config_t s_config;

/**
 * @brief Create and configure a console display configuration instance.
 * @param[in] baud_rate Baud rate for console display output.
 * @return Pointer to the static console display configuration instance.
 */
display_config_t *console_config_create(uint32_t baud_rate) {
    s_config.baud_rate = baud_rate;
    return &s_config;
}

/**
 * @brief Initialize the console display driver with the provided configuration.
 * @param[in] p_config Pointer to the display configuration structure.
 */
static void console_display_init(display_config_t *p_config) {
    if (p_config != NULL) {
        struct display_config_s *p_concrete = (struct display_config_s *)p_config;
        (void)p_concrete; 
    }
    (void)p_config;
}

/**
 * @brief Draw a pixel by printing its coordinates and color to the standard console.
 * @param[in] x X coordinate of the pixel.
 * @param[in] y Y coordinate of the pixel.
 * @param[in] color Color value of the pixel.
 */
static void console_display_draw_pixel(uint16_t x, uint16_t y, uint8_t color) {
    (void)x; 
    (void)y; 
    (void)color; 
    printf("[Console] Drawing pixel at (%u,%u) with color %u\n", x, y, color);
}

/**
 * @brief Instance of the console display interface implementing i_display_t operations.
 */
i_display_t console_display = {
    .init = console_display_init,
    .draw_pixel = console_display_draw_pixel
};
