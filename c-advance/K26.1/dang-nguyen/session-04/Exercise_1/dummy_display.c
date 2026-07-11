#include "dummy_display.h"

static void dummy_display_init(st_display_config_t *p_config);
static void dummy_display_draw_pixel(uint16_t x, uint16_t y, uint8_t colour);

st_i_display_t dummy_display = {
    .init       = &dummy_display_init,
    .draw_pixel = &dummy_display_draw_pixel
};

static uint32_t dummy_draw_count;

static void dummy_display_init(st_display_config_t *p_config)
{
    (void)p_config;
    dummy_draw_count = 0U;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
static void dummy_display_draw_pixel(uint16_t x, uint16_t y, uint8_t colour)
{
    (void)x;
    (void)y;
    (void)colour;

    dummy_draw_count++;
}

uint32_t get_draw_count()
{
    return dummy_draw_count;
}