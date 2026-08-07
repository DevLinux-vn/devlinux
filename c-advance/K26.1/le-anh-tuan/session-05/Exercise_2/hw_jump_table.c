/**
 * @file hw_jump_table.c
 * @brief UI menu dispatcher implemented using a function-pointer jump table.
 *
 * This program demonstrates an O(1) UI dispatcher using an array of
 * function pointers. The jump table is explicitly placed in the custom
 * ".my_dispatch_table" section.
 */

#include <stdint.h>
#include <stdio.h>

/**
 * @brief UI menu identifiers.
 */
typedef enum
{
    MENU_MAIN = 0,
    MENU_SETTINGS,
    MENU_ABOUT,
    MENU_COUNT
} menu_id_t;

/**
 * @brief Function pointer type for UI menu handlers.
 *
 * @param page_id Identifier of the UI page being drawn.
 */
typedef void (*ui_handler_t)(uint8_t page_id);

/**
 * @brief Draw the main menu.
 *
 * @param page_id Identifier of the UI page.
 */
static void draw_menu(uint8_t page_id)
{
    (void)page_id;

    printf("Drawing Main Menu...\n");
}

/**
 * @brief Draw the settings menu.
 *
 * @param page_id Identifier of the UI page.
 */
static void draw_settings(uint8_t page_id)
{
    (void)page_id;

    printf("Drawing Settings Menu...\n");
}

/**
 * @brief Draw the about menu.
 *
 * @param page_id Identifier of the UI page.
 */
static void draw_about(uint8_t page_id)
{
    (void)page_id;

    printf("Drawing About Menu...\n");
}

/**
 * @brief Jump table containing all UI menu handlers.
 *
 * The table is const-qualified so that the function pointers cannot be
 * modified at runtime. It is explicitly placed in the
 * ".my_dispatch_table" section.
 */
static ui_handler_t const p_dispatch_table[MENU_COUNT]
    __attribute__((section(".my_dispatch_table"))) =
{
    draw_menu,
    draw_settings,
    draw_about
};

/**
 * @brief Dispatch a UI menu request.
 *
 * The function validates the supplied menu index before accessing the
 * jump table. It also checks the selected function pointer for NULL
 * before invoking it.
 *
 * @param menu_index Index of the requested menu.
 */
void dispatch_ui(uint8_t menu_index)
{
    ui_handler_t p_handler = NULL;

    if (menu_index < (uint8_t)MENU_COUNT)
    {
        p_handler = p_dispatch_table[menu_index];

        if (p_handler != NULL)
        {
            p_handler(menu_index);
        }
        else
        {
            printf("Error: Invalid menu index!\n");
        }
    }
    else
    {
        printf("Error: Invalid menu index!\n");
    }
}

/**
 * @brief Program entry point.
 *
 * Tests all valid menu indices followed by one invalid index.
 *
 * @return Zero on successful program termination.
 */
int main(void)
{
    dispatch_ui((uint8_t)MENU_MAIN);
    dispatch_ui((uint8_t)MENU_SETTINGS);
    dispatch_ui((uint8_t)MENU_ABOUT);

    dispatch_ui(UINT8_C(99));

    return 0;
}