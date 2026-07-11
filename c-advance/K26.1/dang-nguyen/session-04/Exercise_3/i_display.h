#ifndef I_DISPLAY_H
#define I_DISPLAY_H

#include <stdio.h>
#include <stdint.h>

typedef struct st_display_config    st_display_config_t;

typedef struct st_i_display
{
    void (*init)(st_display_config_t *p_config);
    void (*draw_pixel)(uint16_t x, uint16_t y, uint8_t colour);
} st_i_display_t;

#endif