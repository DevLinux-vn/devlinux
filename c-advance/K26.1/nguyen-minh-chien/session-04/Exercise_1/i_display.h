#ifndef I_DISPLAY_H
#define I_DISPLAY_H
#include <stdint.h>
/**
 * @brief Opaque handle to the display configuration data.
 *        The internal structure is hidden and defined only in the
 *        corresponding driver's .c file (data hiding / encapsulation).
 */
typedef struct display_config_s display_config_t;
/**
 * @brief Hardware Abstraction Layer interface for a display driver.
 *        Any concrete display (console, dummy, real hardware...) must
 *        provide implementations matching these function pointers.
 */
typedef struct i_display_s 
{
    void (*init)(display_config_t *p_config);
    void (*draw_pixel)(uint16_t x, uint16_t y, uint8_t color);
} i_display_t;

#endif /*I_DISPLAY_H*/