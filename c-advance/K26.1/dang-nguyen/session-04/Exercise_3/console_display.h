#ifndef CONSOLE_DISPLAY_H
#define CONSOLE_DISPLAY_H

#include "i_display.h"

st_display_config_t *console_config_create(uint32_t baud_rate);
void console_config_destroy(st_display_config_t *p_dlp_cfg);

extern st_i_display_t console_display;

#endif