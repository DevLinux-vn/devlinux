/**
 * @file console_display.h
 * @brief Console display driver interface.
 * @author Tran Doan Trung
 * @date 2026-11-21
 */
#ifndef CONSOLE_DISPLAY_H
#define CONSOLE_DISPLAY_H

#include "i_display.h"

/**
 * @brief Create and configure a console display configuration instance.
 * @param[in] baud_rate Baud rate for the console display communication.
 * @return Pointer to the created display configuration structure.
 */
display_config_t *console_config_create(uint32_t baud_rate);

/**
 * @brief Console display driver interface instance.
 */
extern i_display_t console_display;

#endif /* CONSOLE_DISPLAY_H */
