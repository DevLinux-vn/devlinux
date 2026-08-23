#include <stdio.h>
#include "i_display.h"
#include "console_display.h"
#include "dummy_display.h"

void draw_rectangle(i_display_t *disp)
{
    if ((disp != NULL) && (disp->draw_pixel != NULL))
    {
        uint16_t x;
        uint16_t y;

        for (y = 0U; y < 2U; y++)
        {
            for (x = 0U; x < 2U; x++)
            {
                disp->draw_pixel(x, y, 1U);
            }
        }
    }
}

int main(void)
{
    display_config_t *p_console_cfg = console_config_create(9600U);

    if (console_display.init != NULL)
    {
        console_display.init(p_console_cfg);
    }
    draw_rectangle(&console_display);

    if (dummy_display.init != NULL)
    {
        dummy_display.init(NULL);
    }
    draw_rectangle(&dummy_display);

    printf("Dummy display was called %u times.\n", dummy_display_get_draw_count());

    return 0;
}