#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "console_display.h"
#include "dummy_display.h"
#include "i_display.h"

/**
 * @file main.c
 * @brief Application demonstrating polymorphism and encapsulation in C.
 */

/**
 * @brief Draw a two-by-two rectangle using a generic display interface.
 *
 * The function depends only on i_display_t and does not know which concrete
 * display implementation is being used.
 *
 * Four pixels are drawn at the following coordinates:
 * - (0, 0)
 * - (1, 0)
 * - (0, 1)
 * - (1, 1)
 *
 * @param[in] p_display Pointer to a display interface.
 */
static void draw_rectangle(i_display_t *p_display)
{
    
    if ((p_display != NULL) && (p_display->draw_pixel != NULL))
    {
        uint16_t x;
        uint16_t y;
        for (y = 0U; y < 2U; ++y)
        {
            for (x = 0U; x < 2U; ++x)
            {
                p_display->draw_pixel(x, y, 1U);
            }
        }
    }
}

/**
 * @brief Program entry point.
 *
 * @return Zero when the program finishes successfully.
 */
int main(void)
{
    display_config_t *p_console_config;
    uint32_t dummy_count;

    p_console_config = console_config_create(115200U);

    if ((p_console_config != NULL) && (console_display.init != NULL))
    {
        console_display.init(p_console_config);
    }

    draw_rectangle(&console_display);

    if (dummy_display.init != NULL)
    {
        /*
         * The dummy driver does not require a configuration object.
         * Its init implementation safely accepts NULL.
         */
        dummy_display.init(NULL);
    }

    draw_rectangle(&dummy_display);

    dummy_count = dummy_display_get_draw_count();

    (void)printf("Dummy display was called %lu times.\n",
                 (unsigned long)dummy_count);

    return 0;
}
