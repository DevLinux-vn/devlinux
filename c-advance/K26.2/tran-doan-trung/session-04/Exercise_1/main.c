/**
 * @file main.c
 * @brief Main application demonstrating polymorphic display driver usage.
 * @author Tran Doan Trung
 * @date 2026-11-21
 */

#include "dummy_display.h"
#include "console_display.h"

#include <stdio.h>
#include <stdint.h>

/**
 * @brief Draws a 2x2 rectangle on the specified display interface.
 * @param[in] p_disp Pointer to the display interface instance.
 */
static void draw_rectangle(i_display_t *p_disp) {
    if ((p_disp != NULL) && (p_disp->draw_pixel != NULL)) {
        p_disp->draw_pixel(0U, 0U, 1U);
        p_disp->draw_pixel(1U, 0U, 1U);
        p_disp->draw_pixel(0U, 1U, 1U);
        p_disp->draw_pixel(1U, 1U, 1U);
    }
}

/**
 * @brief Main entry point demonstrating display interface polymorphism.
 * @return 0 on success.
 */
int main(void) {
    /** @brief Configuration for the console display. */
    display_config_t *console_config = console_config_create(115200);
    if (console_display.init != NULL) {
        console_display.init(console_config);
    }
    draw_rectangle(&console_display);

    /** @brief Configuration for the dummy display. */
    display_config_t *dummy_config = NULL; /* No specific configuration for dummy display */
    if (dummy_display.init != NULL) {
        dummy_display.init(dummy_config);
    }
    draw_rectangle(&dummy_display);

    (void)printf("Dummy display was called %u times.\n", (unsigned int)dummy_draw_count);

    return 0;
}
