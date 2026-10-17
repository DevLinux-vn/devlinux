#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "console_display.h"
#include "dummy_display.h"
#include "i_display.h"

/**
 * @brief Draw a 2-by-2 rectangle using an abstract display interface.
 *
 * The function depends only on i_display_t and therefore does not know
 * which concrete display implementation is being used.
 *
 * @param p_disp Pointer to the display interface.
 */
static void draw_rectangle(i_display_t *p_disp)
{
    uint16_t x;
    uint16_t y;
    const uint8_t color = 1U;

    if ((p_disp != NULL) && (p_disp->draw_pixel != NULL))
    {
        for (y = 0U; y < 2U; y++)
        {
            for (x = 0U; x < 2U; x++)
            {
                p_disp->draw_pixel(x, y, color);
            }
        }
    }
}

/**
 * @brief Program entry point.
 *
 * Demonstrates the same application drawing operation with two different
 * display implementations through a common polymorphic interface.
 *
 * @return Zero on successful completion.
 */
int main(void)
{
    display_config_t *p_config;

    p_config = console_config_create(115200U);

    if ((p_config != NULL) && (console_display.init != NULL))
    {
        console_display.init(p_config);
    }

    draw_rectangle(&console_display);

    if (dummy_display.init != NULL)
    {
        dummy_display.init(p_config);
    }

    draw_rectangle(&dummy_display);

    (void)printf(
        "Dummy display was called %" PRIu32 " times.\n",
        dummy_display_get_draw_count());

    return 0;
}