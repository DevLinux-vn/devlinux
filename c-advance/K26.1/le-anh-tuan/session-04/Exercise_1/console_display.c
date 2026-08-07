#include "console_display.h"

#include <inttypes.h>
#include <stdio.h>

/**
 * @brief Concrete console display configuration.
 *
 * The complete definition exists only in this translation unit,
 * keeping the configuration hidden from application code.
 */
struct display_config_s
{
    uint32_t baud_rate;
};

/**
 * @brief Initialize the console display.
 *
 * @param p_config Pointer to the opaque console configuration.
 */
static void console_display_init(display_config_t *p_config)
{
    struct display_config_s *p_console_config = p_config;

    if (p_console_config != NULL)
    {
        /*
         * The baud rate would normally be used to initialize hardware.
         * Reading it here demonstrates access to the private configuration
         * without producing additional output.
         */
        (void)p_console_config->baud_rate;
    }
}

/**
 * @brief Draw one pixel using the console display.
 *
 * @param x Horizontal pixel coordinate.
 * @param y Vertical pixel coordinate.
 * @param color Pixel color value.
 */
static void console_display_draw_pixel(
    uint16_t x,
    uint16_t y,
    uint8_t color)
{
    (void)printf(
        "[Console] Drawing pixel at (%" PRIu16 ",%" PRIu16
        ") with color %" PRIu8 "\n",
        x,
        y,
        color);
}

/**
 * @brief Create a console display configuration.
 *
 * A statically allocated object is used, so dynamic allocation and
 * deallocation are unnecessary for this exercise.
 *
 * @param baud_rate Baud rate for the console display.
 *
 * @return Pointer to the opaque console configuration.
 */
display_config_t *console_config_create(uint32_t baud_rate)
{
    static struct display_config_s config;

    config.baud_rate = baud_rate;

    return &config;
}

/**
 * @brief Global console display interface.
 */
i_display_t console_display =
{
    .init = console_display_init,
    .draw_pixel = console_display_draw_pixel
};