#include <logger_module.h>

#define APP_SUCCESS     (0)

int main()
{
    printf("=== Exercise 3: Industrial Logger ===\n");
    printf("Compiled with LOG_LEVEL_MAX = %d\n\n", LOG_LEVEL_MAX);

    LOG_INFO("%s", "System boot. Build time: 12:00:00\n");
    LOG_DEBUG("%s", "Discovered 4 sensors on I2C bus.\n");
    LOG_WARNING("%s", "Sensor 2 reading is unstable.\n");
    LOG_ERROR("%s", "Watchdog timeout! Rebooting in 500 ms.\n");
    LOG_INFO("%s", "Boot sequence complete.\n");
    return APP_SUCCESS;
}