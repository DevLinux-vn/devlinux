#include "dummy_display.h"

#include <stddef.h>

/**
 * @brief Number of calls made to the dummy pixel-drawing function.
 *
 * The counter is private to this translation unit.
 */
static uint32_t dummy_draw_count = 0U;

/**
 * @brief Initialize the dummy display.
 *
 * The dummy driver requires no hardware configuration, so the supplied
 * opaque configuration pointer is intentionally unused.
 *
 * @param p_config Pointer to a display configuration.
 */
static void dummy_display_init(display_config_t *p_config)
{
    if (p_config != NULL)
    {
        /*
         * No initialization is necessary for the mock implementation.
         */
    }

    dummy_draw_count = 0U;
}

/**
 * @brief Simulate drawing a pixel using the dummy display.
 *
 * No actual drawing takes place. The function records each call so that
 * application behavior can be verified.
 *
 * @param x Horizontal pixel coordinate.
 * @param y Vertical pixel coordinate.
 * @param color Pixel color value.
 */
static void dummy_display_draw_pixel(
    uint16_t x,
    uint16_t y,
    uint8_t color)
{
    (void)x;
    (void)y;
    (void)color;

    dummy_draw_count++;
}

/**
 * @brief Return the dummy display invocation counter.
 *
 * @return Number of pixels passed to the dummy display.
 */
uint32_t dummy_display_get_draw_count(void)
{
    return dummy_draw_count;
}

/**
 * @brief Global dummy display interface.
 */
i_display_t dummy_display =
{
    .init = dummy_display_init,
    .draw_pixel = dummy_display_draw_pixel
};