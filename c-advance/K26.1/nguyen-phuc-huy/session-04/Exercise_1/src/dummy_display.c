#include <stddef.h>
#include <stdint.h>

#include "dummy_display.h"

/**
 * @file dummy_display.c
 * @brief Concrete dummy display implementation.
 */

/**
 * @brief Number of pixels requested from the dummy display.
 */
static uint32_t dummy_draw_count = 0U;

/**
 * @brief Initialize the dummy display.
 *
 * The dummy display has no hardware configuration, so the configuration
 * pointer is intentionally unused.
 *
 * @param[in,out] p_config Pointer to a display configuration, or NULL.
 */
static void dummy_display_init(display_config_t *p_config)
{
    (void)p_config;

    dummy_display_reset_draw_count();
}

/**
 * @brief Simulate drawing a pixel by incrementing a counter.
 *
 * @param[in] x     Horizontal pixel coordinate.
 * @param[in] y     Vertical pixel coordinate.
 * @param[in] color Pixel color value.
 */
static void dummy_display_draw_pixel(uint16_t x,
                                     uint16_t y,
                                     uint8_t color)
{
    (void)x;
    (void)y;
    (void)color;

    ++dummy_draw_count;
}

/**
 * @brief Get the number of dummy pixel-drawing calls.
 *
 * @return Number of times dummy_display_draw_pixel() has been called.
 */
uint32_t dummy_display_get_draw_count(void)
{
    return dummy_draw_count;
}

/**
 * @brief Reset the dummy display draw counter.
 */
void dummy_display_reset_draw_count(void)
{
    dummy_draw_count = 0U;
}

/**
 * @brief Dummy display interface object.
 */
i_display_t dummy_display =
{
    dummy_display_init,
    dummy_display_draw_pixel
};
