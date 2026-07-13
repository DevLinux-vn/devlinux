#ifndef HW_JUMP_TABLE_H
#define HW_JUMP_TABLE_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/**
 * @brief UI menu identifiers.
 */
typedef enum
{
    MENU_MAIN = 0,   /**< Main menu page. */
    MENU_SETTING,    /**< Settings menu page. */
    MENU_ABOUT,      /**< About menu page. */
    MENU_COUNT       /**< Total number of valid menus. */
} menu_t;

/**
 * @brief Draw the Main menu.
 * @param[in] index Menu page index.
 */
void draw_menu(uint8_t index);

/**
 * @brief Draw the Settings menu.
 * @param[in] index Menu page index.
 */
void draw_settings(uint8_t index);

/**
 * @brief Draw the About menu.
 * @param[in] index Menu page index.
 */
void draw_about(uint8_t index);

/**
 * @brief Function pointer type for UI menu handlers.
 * @param[in] index Menu page index.
 */
typedef void (*menu_handler_t)(uint8_t index);

/**
 * @brief UI menu dispatch table.
 *
 * Constant jump table that maps each menu identifier to its
 * corresponding drawing function. The table is indexed using
 * values of ::menu_t.
 */
    __attribute__((section(".my_dispatch_table"))) 
    static const menu_handler_t p_draw[] =
    {
        [MENU_MAIN]    = draw_menu,
        [MENU_SETTING] = draw_settings,
        [MENU_ABOUT]   = draw_about
    };

/**
 * @brief Dispatch a UI menu request.
 * @param[in] menu_index Menu identifier to dispatch.
 */
void dispatch_ui(uint8_t menu_index);

#endif /* HW_JUMP_TABLE_H */