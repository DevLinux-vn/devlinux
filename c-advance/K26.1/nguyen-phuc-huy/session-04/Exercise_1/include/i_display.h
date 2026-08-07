#ifndef I_DISPLAY_H
#define I_DISPLAY_H

#include <stdint.h>

/**
 * @file i_display.h
 * @brief Public polymorphic display interface.
 */

/**
 * @brief Opaque display configuration type.
 *
 * The structure is intentionally incomplete in this header. Its internal
 * members are defined only by the concrete display implementation.
 */
typedef struct display_config_s display_config_t;

/**
 * @brief Interface representing a generic display driver.
 *
 * Concrete display drivers provide implementations for the function
 * pointers in this structure.
 */
typedef struct i_display_s
{
    /**
     * @brief Initialize the display driver.
     *
     * @param[in,out] p_config Pointer to an opaque display configuration.
     */
    void (*init)(display_config_t *p_config);

    /**
     * @brief Draw one pixel on the display.
     *
     * @param[in] x     Horizontal pixel coordinate.
     * @param[in] y     Vertical pixel coordinate.
     * @param[in] color Pixel color value.
     */
    void (*draw_pixel)(uint16_t x, uint16_t y, uint8_t color);
} i_display_t;

#endif /* I_DISPLAY_H */