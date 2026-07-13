#include "hw_jump_table.h"

void draw_menu(uint8_t index)
{
    (void)index;
    printf("Drawing Main Menu...\n");
}

void draw_settings(uint8_t index)
{
    (void)index;
    printf("Drawing Settings Menu...\n");
}
void draw_about(uint8_t index)
{
    (void)index;
    printf("Drawing About Menu...\n");
}


void dispatch_ui(uint8_t menu_index)
{
    if (menu_index >= MENU_COUNT)
    {
        printf("Error: Invalid menu index!\n");
        return;
    }
    else
    {
        p_draw[menu_index](menu_index);
    }

    return;
}