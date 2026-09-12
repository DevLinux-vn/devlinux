#include "i_display.h"
#include "console_display.h"
#include "dummy_display.h"

static void draw_rectangle(i_display_t *p_disp)
{
    if (p_disp == NULL)
    {
        return;
    }

    if (p_disp->draw_pixel != NULL)
    {
        p_disp->draw_pixel(0, 0, 1);
        p_disp->draw_pixel(1, 0, 1);
        p_disp->draw_pixel(0, 1, 1);
        p_disp->draw_pixel(1, 1, 1);
    }
}

int main(void)
{
    display_config_t *p_cfg;
    p_cfg = console_config_create(115200);
    if (p_cfg == NULL)
    {
        return -1;
    }

    /* Console test */
    if (console_display.init != NULL)
    {
        console_display.init(p_cfg);
    }
    draw_rectangle(&console_display);


    /* Dummy test */
    if (dummy_display.init != NULL)
    {
        dummy_display.init(p_cfg);
    }
    draw_rectangle(&dummy_display);

    printf("Dummy display was called %u times.\n", dummy_get_draw_count());

    free(p_cfg);

    return 0;
}