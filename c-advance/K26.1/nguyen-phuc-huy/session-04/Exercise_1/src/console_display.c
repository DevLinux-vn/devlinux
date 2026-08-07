#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "console_display.h"

/**
 * @file console_display.c
 * @brief Concrete console display implementation.
 */

/**
 * @brief Private console display configuration.
 *
 * This complete structure definition is visible only in this translation
 * unit. Application code can only use a pointer to display_config_t.
 */
struct display_config_s
{
    uint32_t baud_rate;
    uint8_t is_initialized;
};

/**
 * @brief Initialize the console display.
 *
 * The opaque configuration pointer is converted back to its concrete type
 * inside this implementation file.
 *
 * @param[in,out] p_config Pointer to the console display configuration.
 */
static void console_display_init(display_config_t *p_config)
{
    struct display_config_s *p_console_config = p_config;

    if (p_console_config != NULL)
    {
        p_console_config->is_initialized = 1U;
    }
}

/**
 * @brief Simulate drawing one pixel by printing to the terminal.
 *
 * @param[in] x     Horizontal pixel coordinate.
 * @param[in] y     Vertical pixel coordinate.
 * @param[in] color Pixel color value.
 */
static void console_display_draw_pixel(uint16_t x,
                                       uint16_t y,
                                       uint8_t color)
{
    (void)printf("[Console] Drawing pixel at (%u,%u) with color %u\n",
                 (unsigned int)x,
                 (unsigned int)y,
                 (unsigned int)color);
}

/**
 * @brief Create or configure the console display configuration.
 *
 * A statically allocated object is used, so no dynamic memory allocation
 * or explicit destruction function is required.
 *
 * @param[in] baud_rate Baud rate for the simulated console display.
 *
 * @return Pointer to an opaque console display configuration.
 */
display_config_t *console_config_create(uint32_t baud_rate)
{
    static struct display_config_s console_config =
    {
        0U,
        0U
    };

    console_config.baud_rate = baud_rate;
    console_config.is_initialized = 0U;

    return &console_config;
}

/**
 * @brief Console display interface object.
 */
i_display_t console_display =
{
    console_display_init,
    console_display_draw_pixel
};
