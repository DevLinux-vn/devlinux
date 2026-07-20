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
 * @brief Function pointer type for UI menu handlers.
 * @param[in] index Menu page index.
 */
typedef void (*menu_handler_t)(uint8_t index);

/**
 * @brief Dispatch a UI menu request.
 * @param[in] menu_index Menu identifier to dispatch.
 */
void dispatch_ui(uint8_t menu_index);

#endif /* HW_JUMP_TABLE_H */