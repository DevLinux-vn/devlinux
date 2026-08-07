#ifndef I_DISPLAY_H
#define I_DISPLAY_H

#include <stdint.h>

/**
 * @brief Opaque display configuration type.
 *
 * The structure definition is intentionally hidden from application code.
 */
typedef struct display_config_s display_config_t;

/**
 * @brief Generic display hardware abstraction interface.
 *
 * Concrete display drivers provide implementations for the function
 * pointers contained in this structure.
 */
typedef struct i_display_s
{
    /**
     * @brief Initialize the display driver.
     *
     * @param p_config Pointer to the opaque display configuration.
     */
    void (*init)(display_config_t *p_config);

    /**
     * @brief Draw one pixel on the display.
     *
     * @param x Horizontal pixel coordinate.
     * @param y Vertical pixel coordinate.
     * @param color Pixel color value.
     */
    void (*draw_pixel)(uint16_t x, uint16_t y, uint8_t color);
} i_display_t;

#endif /* I_DISPLAY_H */