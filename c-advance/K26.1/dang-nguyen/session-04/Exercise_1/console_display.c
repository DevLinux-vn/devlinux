#include <stdlib.h>
#include <string.h>
#include "console_display.h"

typedef struct st_display_config
{
    uint32_t baud_rate;
} st_display_config_t;

/**
 * @brief Initialize the console display.
 *
 * Converts the opaque configuration handle to the console display
 * configuration type and validates it before initialization.
 *
 * @param[in] p_cfg_hdl Opaque handle to the console display configuration.
 *
 * @return Result of the initialization.
 * @retval DL_RET_OK            Initialization completed successfully.
 * @retval DL_RET_INVALID_PARAM The configuration handle is NULL.
 */
static e_errcode_t console_display_init(const display_config_handle_t p_cfg_hdl);

/**
 * @brief Draw a pixel on the console display.
 *
 * Prints the pixel coordinates and colour value to the console.
 *
 * @param[in] x      Horizontal coordinate of the pixel.
 * @param[in] y      Vertical coordinate of the pixel.
 * @param[in] colour Pixel colour value.
 */
static void console_display_draw_pixel(const uint16_t x, const uint16_t y, const uint8_t colour);

/**
 * @brief Console display interface instance.
 * Implements polymorphic display driver for terminal output.
 */
const st_i_display_t console_display = {
    .init       = &console_display_init,
    .draw_pixel = &console_display_draw_pixel
};

static e_errcode_t console_display_init(const display_config_handle_t p_cfg_hdl)
{
    st_display_config_t *p_dpl_cfg = (st_display_config_t *)p_cfg_hdl;
    e_errcode_t         ret        = DL_RET_OK;

    if (NULL == p_dpl_cfg)
    {
        printf("[%s]: Invalid p_config\n", __func__);
        ret = DL_RET_INVALID_PARAM;
    }

    return ret;
}

static void console_display_draw_pixel(const uint16_t x, const uint16_t y, const uint8_t colour)
{
    printf("[Console] Drawing pixel at (%u, %u) with color %u\n", x, y, colour);
}

display_config_handle_t console_config_create(const uint32_t baud_rate)
{
    static st_display_config_t dpl_cfg = {0};

    dpl_cfg.baud_rate = baud_rate;
    
    return (display_config_handle_t)&dpl_cfg;
}