#include "hw_jump_table.h"

static void draw_menu(uint8_t index)
{
    (void)index;
    printf("Drawing Main Menu...\n");
}

static void draw_settings(uint8_t index)
{
    (void)index;
    printf("Drawing Settings Menu...\n");
}
static void draw_about(uint8_t index)
{
    (void)index;
    printf("Drawing About Menu...\n");
}

__attribute__((section(".my_dispatch_table"))) 
static const menu_handler_t p_draw[] =
{
    [MENU_MAIN]    = draw_menu,
    [MENU_SETTING] = draw_settings,
    [MENU_ABOUT]   = draw_about
};

void dispatch_ui(uint8_t menu_index)
{
    if (menu_index >= MENU_COUNT)
    {
        printf("Error: Invalid menu index!\n");
        return;
    }

    if (p_draw[menu_index] == NULL)
    {
        printf("Error: NULL handler!\n");
        return;
    }

    p_draw[menu_index](menu_index);
}