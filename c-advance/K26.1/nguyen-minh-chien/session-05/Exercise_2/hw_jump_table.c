#include <stdio.h>
#include <stdint.h>

/**
 * @brief typedef for a function pointer that takes a uint8_t (page ID) and returns void
 */
typedef void (*ui_handler_t)(uint8_t);

/**
 * @brief Định danh các trang menu hợp lệ trên thiết bị.
 */
typedef enum
{
    MENU_ID_MAIN = 0,
    MENU_ID_SETTINGS,
    MENU_ID_ABOUT,
    MENU_ID_COUNT
} menu_id_t;

/**
 * @brief Vẽ màn hình Main Menu.
 * @param menu_index Chỉ số trang.
 */
void draw_menu(uint8_t menu_index)
{
    (void)menu_index; /* tránh warning unused-parameter */
    printf("Drawing Main Menu...\n");
}

/**
 * @brief Vẽ màn hình Settings Menu.
 * @param menu_index Chỉ số trang.
 */
void draw_settings(uint8_t menu_index)
{
    (void)menu_index; /* tránh warning unused-parameter */
    printf("Drawing Settings Menu...\n");
}

/**
 * @brief Vẽ màn hình About Menu.
 * @param menu_index Chỉ số trang.
 */
void draw_about(uint8_t menu_index)
{
    (void)menu_index; /* tránh warning unused-parameter */
    printf("Drawing About Menu...\n");
}

/**
 * @brief Bảng jump table (O(1)) ánh xạ menu_index sang hàm vẽ tương ứng.
 * @note Được đặt tường minh vào section .my_dispatch_table để tối ưu flash.
 */
static const ui_handler_t g_p_dispatch_table[] __attribute__((section(".my_dispatch_table"))) =
{
    [MENU_ID_MAIN] = draw_menu,
    [MENU_ID_SETTINGS] = draw_settings,
    [MENU_ID_ABOUT] = draw_about
};

/**
 * @brief Điều phối UI dựa trên menu_index, có kiểm tra biên.
 * @param menu_index Chỉ số menu cần vẽ.
 */
void dispatch_ui(uint8_t menu_index)
{
    const uint8_t table_size = (uint8_t)(sizeof(g_p_dispatch_table) / sizeof(g_p_dispatch_table[0]));

    if (menu_index < table_size && g_p_dispatch_table[menu_index] != NULL)
    {
        g_p_dispatch_table[menu_index](menu_index); /* gọi hàm qua con trỏ trong bảng */
    }
    else
    {
        printf("Error: Invalid menu index!\n");
    }
}

/**
 * @brief Điểm vào chương trình, kiểm tra điều phối UI với các menu hợp lệ và không hợp lệ.
 * @return 0 when the program exits successfully
 */
int main(void)
{
    dispatch_ui(MENU_ID_MAIN);    /* Main Menu    - hợp lệ */
    dispatch_ui(MENU_ID_SETTINGS); /* Settings     - hợp lệ */
    dispatch_ui(MENU_ID_ABOUT);    /* About        - hợp lệ */
    dispatch_ui(MENU_ID_COUNT);    /* index không hợp lệ */

    return 0;
}