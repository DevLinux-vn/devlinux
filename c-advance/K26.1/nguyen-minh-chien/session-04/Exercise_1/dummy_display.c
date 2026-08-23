#include "dummy_display.h"

/**
 * @brief Static counter tracking how many times draw_pixel has been called.
 *        Scope limited to this file (data hiding).
 */
static uint32_t dummy_draw_count = 0U;

/**
 * @brief Dummy implementation of display init. Does nothing (no real hardware).
 * @param p_config Unused opaque config pointer.
 */
static void dummy_display_init(display_config_t *p_config)
{
    (void)p_config; /* unused parameter, avoid compiler warning */
}

/**
 * @brief Dummy implementation of draw_pixel. Just increments the internal counter.
 * @param x Unused.
 * @param y Unused.
 * @param color Unused.
 */
static void dummy_display_draw_pixel(uint16_t x, uint16_t y, uint8_t color)
{
    (void)x;
    (void)y;
    (void)color;
    dummy_draw_count++;
}

uint32_t dummy_display_get_draw_count(void)
{
    return dummy_draw_count;
}

/**
 * @brief Global dummy display instance, wiring interface function pointers
 *        to the dummy implementations above.
 */
i_display_t dummy_display = {
    .init = dummy_display_init,
    .draw_pixel = dummy_display_draw_pixel
};