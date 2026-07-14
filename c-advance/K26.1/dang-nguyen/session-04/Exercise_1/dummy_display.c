#include "dummy_display.h"

/**
 * @brief Initialize the dummy display.
 *
 * Converts the opaque configuration handle to the dummy display
 * configuration type and initialize drawing count.
 *
 * @param[in] p_cfg_hdl Opaque handle to the dummy display configuration.
 *                      This parameter is unused.
 *
 * @return Result of the initialization.
 * @retval DL_RET_OK            Initialization completed successfully (Always).
 */
static e_errcode_t dummy_display_init(const display_config_handle_t p_cfg_hdl);

/**
 * @brief Simulate drawing a pixel on the dummy display.
 *
 * The pixel parameters are ignored. Each call increments the dummy display
 * draw counter.
 *
 * @param[in] x      Horizontal coordinate of the pixel.
 * @param[in] y      Vertical coordinate of the pixel.
 * @param[in] colour Pixel colour value.
 */
static void dummy_display_draw_pixel(const uint16_t x, const uint16_t y, const uint8_t colour);

const st_i_display_t dummy_display = {
    .init       = &dummy_display_init,
    .draw_pixel = &dummy_display_draw_pixel
};

static uint32_t dummy_draw_count;

static e_errcode_t dummy_display_init(const display_config_handle_t p_cfg_hdl)
{
    (void)p_cfg_hdl;
    dummy_draw_count = 0U;

    return DL_RET_OK;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
static void dummy_display_draw_pixel(const uint16_t x, const uint16_t y, const uint8_t colour)
{
    /* Unused */
    (void)x;
    (void)y;
    (void)colour;

    /* Count the draw times */
    dummy_draw_count++;
}

uint32_t get_draw_count(void)
{
    return dummy_draw_count;
}