#include "hw_jump_table.h"

int main()
{
    dispatch_ui(MENU_MAIN);
    dispatch_ui(MENU_SETTING);
    dispatch_ui(MENU_ABOUT);
    dispatch_ui(MENU_COUNT);

    return 0;
}