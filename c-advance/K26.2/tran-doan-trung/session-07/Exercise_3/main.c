/**
 * @file main.c
 * @brief Application demonstrating Industrial Logger Module.
 */

#include "logger.h"
#include <stdio.h>

/**
 * @brief Main entry point exercising all log severity levels.
 * @return 0 on success.
 */
int main(void)
{
    printf("=== Exercise 3: Industrial Logger ===\n");
    printf("Compiled with LOG_LEVEL_MAX = %d\n\n", LOG_LEVEL_MAX);

    LOG_INFO("System boot. Build time: %s", __TIME__);
    LOG_DEBUG("Discovered %d sensors on I2C bus.", 4);
    LOG_WARNING("Sensor %d reading is unstable.", 2);
    LOG_ERROR("Watchdog timeout! Rebooting in %d ms.", 500);
    LOG_INFO("Boot sequence complete.");

    return 0;
}
