#include <stdlib.h>
#include <string.h>
#include "console_display.h"

struct st_display_config
{
    uint16_t baud_rate;
};

static void console_display_init(st_display_config_t *p_config);
static void console_display_draw_pixel(uint16_t x, uint16_t y, uint8_t colour);

st_i_display_t console_display = {
    .init       = &console_display_init,
    .draw_pixel = &console_display_draw_pixel
};

static void console_display_init(st_display_config_t *p_config)
{
    if (NULL == p_config)
    {
        printf("[%s]: Invalid p_config\n", __func__);
    }
}

static void console_display_draw_pixel(uint16_t x, uint16_t y, uint8_t colour)
{
    printf("[Console] Drawing pixel at (%u, %u) with color %u\n", x, y, colour);
}

st_display_config_t *console_config_create(uint32_t baud_rate)
{
    st_display_config_t *p_dpl_cfg = (st_display_config_t *)malloc(sizeof(st_display_config_t));

    if (NULL != p_dpl_cfg)
    {
        (void)memset(p_dpl_cfg, 0, sizeof(st_display_config_t));
        p_dpl_cfg->baud_rate = baud_rate;
    }

    return p_dpl_cfg;
}

void console_config_destroy(st_display_config_t *p_dlp_cfg)
{
    if (NULL != p_dlp_cfg)
    {
        free(p_dlp_cfg);
    }
}