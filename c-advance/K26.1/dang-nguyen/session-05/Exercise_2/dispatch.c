#include <stdint.h>
#include <stdio.h>
#include "dispatch.h"

/**
 * @brief Draw the main menu.
 *
 * Displays the main menu interface.
 */
static void draw_menu(void);

/**
 * @brief Draw the settings menu.
 *
 * Displays the settings menu interface.
 */
static void draw_settings(void);

/**
 * @brief Draw the about menu.
 *
 * Displays the application information interface.
 */
static void draw_about(void);

/**
 * @brief UI command dispatch table.
 *
 * Maps each value of @ref e_cmd_t to its corresponding UI handler.
 * The table is placed in the `.my_dispatch_table` linker section.
 *
 * @note The array indexes must correspond to the command values defined
 *       by @ref e_cmd_t.
 */
static void __attribute__((section(".my_dispatch_table"))) (*func[CMD_COUNT])(void) =
{
    [MAIN_MENU]     = &draw_menu,
    [SETTINGS_MENU] = &draw_settings,
    [ABOUT_MENU]    = &draw_about
};

static void draw_menu(void)
{
    (void)printf("Drawing Main Menu...\n");
}

static void draw_settings(void)
{
    (void)printf("Drawing Settings Menu...\n");
}

static void draw_about(void)
{
    (void)printf("Drawing About Menu...\n");
}

e_error_code_t dispatch_ui(const uint8_t menu_index)
{
    e_error_code_t ret = ERR_OK;

    if (menu_index < (const uint8_t)CMD_COUNT)
    {
        func[menu_index]();
    }
    else
    {
        printf("Error: Invalid menu index!\n");
        ret = ERR_INVALID_PARAM;
    }

    return ret;
}