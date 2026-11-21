/**
 * @file i_display.h
 * @brief Hardware Abstraction Layer (HAL) interface for display drivers.
 * @author Tran Doan Trung
 * @date 2026-11-21
 */

#ifndef I_DISPLAY_H
#define I_DISPLAY_H

#include <stdint.h>

/**
 * @brief Forward declaration of opaque display configuration structure.
 *
 * Each concrete display implementation defines its own internal configuration fields.
 */
typedef struct display_config_s display_config_t;

/**
 * @brief Interface structure containing function pointers for display drivers.
 */
typedef struct i_display_s {
    /**
     * @brief Initialize the display driver with the specified configuration.
     * @param[in] p_config Pointer to the display configuration structure.
     */
    void (*init)(display_config_t *p_config);

    /**
     * @brief Draw a single pixel at the specified coordinate.
     * @param[in] x X coordinate of the pixel.
     * @param[in] y Y coordinate of the pixel.
     * @param[in] color Color value of the pixel.
     */
    void (*draw_pixel)(uint16_t x, uint16_t y, uint8_t color);
} i_display_t;

#endif /* I_DISPLAY_H */
