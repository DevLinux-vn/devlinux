#include <stdio.h>
#include "logging.h"

#define BUILD_HOUR          "12"
#define BUILD_MINUTE        "00"
#define BUILD_SECOND        "00"

#define BUILD_TIME          BUILD_HOUR":"BUILD_MINUTE":"BUILD_SECOND
#define SENSOR_NUM          (4U)
#define SENSOR_UNSTABLE     (2U)
#define REBOOTING_TIME      (500U)

#define APP_SUCCESS         (0)

int32_t main(void)
{
    printf("=== Exercise 3: Industrial Logger ===\n");

#if defined(LOG_LEVEL_MAX)
    printf("Compiled with LOG_LEVEL_MAX = %d\n\n", LOG_LEVEL_MAX);
#endif

    LOG_INFO("System boot. Build time: %s", BUILD_TIME);
    LOG_DEBUG("Discovered %u sensors on I2C bus.", SENSOR_NUM);
    LOG_WARNING("Sensor %u reading is unstable.", SENSOR_UNSTABLE);
    LOG_ERROR("Watchdog timeout! Rebooting in %u ms.", REBOOTING_TIME);
    LOG_INFO("%s", "Boot sequence complete.");

    return APP_SUCCESS;
}