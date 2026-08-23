#include "console_display.h"
#include <stdio.h>

/**
 * @brief Real (hidden) configuration data for the console display.
 *        Only this file knows this structure's contents — this is what
 *        makes the opaque pointer in i_display.h a true data-hiding mechanism.
 */
struct display_config_s
{
    uint32_t baud_rate; /**< Simulated baud rate for the console "hardware". */
};

/* Static storage for one console config instance (static allocation). */
static struct display_config_s console_config_storage;

display_config_t *console_config_create(uint32_t baud_rate)
{
    console_config_storage.baud_rate = baud_rate;
    return &console_config_storage;
}

/**
 * @brief Console implementation of display init. Casts the opaque pointer
 *        back to the real struct to read the configuration.
 * @param p_config Pointer to the console's configuration data.
 */
static void console_display_init(display_config_t *p_config)
{
    if (p_config != NULL)
    {
        struct display_config_s *p_real_cfg = (struct display_config_s *)p_config;
        printf("[Console] Initialized with baud rate %u\n", p_real_cfg->baud_rate);
    }
}

/**
 * @brief Console implementation of draw_pixel. Prints a log line simulating
 *        a pixel draw operation.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param color Color value.
 */
static void console_display_draw_pixel(uint16_t x, uint16_t y, uint8_t color)
{
    printf("[Console] Drawing pixel at (%u,%u) with color %u\n", x, y, color);
}

/**
 * @brief Global console display instance.
 */
i_display_t console_display = {
    .init = console_display_init,
    .draw_pixel = console_display_draw_pixel
};