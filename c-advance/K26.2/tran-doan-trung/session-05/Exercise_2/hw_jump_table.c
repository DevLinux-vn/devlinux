/**
 * @file hw_jump_table.c
 * @brief UI menu dispatcher implemented using an O(1) jump table.
 * @details Uses an array of function pointers placed explicitly in a custom
 *          memory section (.my_dispatch_table) to optimize execution and RAM footprint.
 * @author Tran Doan Trung
 * @date 2026-11-21
 */

#include <stdio.h>
#include <stdint.h>

/**
 * @brief Enumeration of available UI menu identifiers.
 */
typedef enum {
    MENU_MAIN = 0,   /**< Main menu screen index */
    MENU_SETTINGS,   /**< Settings configuration menu index */
    MENU_ABOUT,      /**< About information menu index */
    MENU_COUNT       /**< Total number of menu screens (used for bounds checking) */
} menu_index_t;

/**
 * @brief Function pointer type for menu rendering handlers.
 *
 * @param[in] page_id Page identifier passed to the menu handler.
 */
typedef void (*menu_t)(uint8_t page_id);

/**
 * @brief Mock handler to render the Main Menu screen.
 *
 * @param[in] page_id Identifier of the page being drawn (unused).
 */
static void draw_menu(uint8_t page_id) {
    (void)page_id;
    printf("Drawing Main Menu...\n");
}

/**
 * @brief Mock handler to render the Settings Menu screen.
 *
 * @param[in] page_id Identifier of the page being drawn (unused).
 */
static void draw_settings(uint8_t page_id) {
    (void)page_id;
    printf("Drawing Settings Menu...\n");
}

/**
 * @brief Mock handler to render the About Menu screen.
 *
 * @param[in] page_id Identifier of the page being drawn (unused).
 */
static void draw_about(uint8_t page_id) {
    (void)page_id;
    printf("Drawing About Menu...\n");
}

/**
 * @brief Constant jump table mapping menu indices to handler functions.
 * @note Placed in the custom section ".my_dispatch_table" to simulate flash placement.
 */
static const menu_t p_menu_table[MENU_COUNT] __attribute__((section(".my_dispatch_table"))) = {
    [MENU_MAIN]     = draw_menu,
    [MENU_SETTINGS] = draw_settings,
    [MENU_ABOUT]    = draw_about
};

/**
 * @brief Dispatches UI rendering to the corresponding menu handler.
 * @details Performs bounds-checking against MENU_COUNT and NULL-pointer validation
 *          before invoking the handler function.
 *
 * @param[in] menu_index Identifier of the menu to be dispatched.
 */
static void dispatch_ui(uint8_t menu_index) {
    if ((menu_index < (uint8_t)MENU_COUNT) && (p_menu_table[menu_index] != NULL)) {
        p_menu_table[menu_index](menu_index);
    } else {
        printf("Error: Invalid menu index!\n");
    }
}

/**
 * @brief Main function testing the menu dispatch jump table.
 *
 * @return int 0 upon successful execution.
 */
int main(void) {
    dispatch_ui((uint8_t)MENU_MAIN);      /* 0 -> Drawing Main Menu... */
    dispatch_ui((uint8_t)MENU_SETTINGS);  /* 1 -> Drawing Settings Menu... */
    dispatch_ui((uint8_t)MENU_ABOUT);     /* 2 -> Drawing About Menu... */
    dispatch_ui(99U);                     /* 99 -> Error: Invalid menu index! */
    return 0;
}
