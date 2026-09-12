#ifndef I_DISPLAY_H
#define I_DISPLAY_H

#include <stdint.h>

/**
 * @brief Opaque display configuration type.
 *
 * Forward declaration of the display configuration structure.
 * The actual structure definition is hidden in the corresponding
 * source file to provide encapsulation and prevent direct access
 * to the configuration data.
 */
typedef struct display_config_s display_config_t;

/**
 * @brief Hardware abstraction interface for display drivers.
 *
 * This structure defines a generic display interface using
 * function pointers. Different display drivers (e.g., console,
 * LCD, OLED, or dummy display) implement these operations,
 * allowing application code to interact with displays
 * polymorphically.
 */
typedef struct i_display_s
{
    /**
     * @brief Initializes the display driver.
     *
     * @param[in] p_config Pointer to the display configuration object.
     */
    void (*init)(display_config_t *p_config);

    /**
     * @brief Draws a single pixel on the display.
     *
     * @param[in] x Horizontal pixel coordinate.
     * @param[in] y Vertical pixel coordinate.
     * @param[in] color Pixel color value.
     */
    void (*draw_pixel)(uint16_t x, uint16_t y, uint8_t color);

} i_display_t;

#endif /* I_DISPLAY_H */