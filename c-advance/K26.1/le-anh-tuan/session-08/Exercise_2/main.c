/**
 * @file main.c
 * @brief Demonstrates a safe fix for a format string vulnerability.
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Logs the online status of a device safely.
 *
 * The device name is treated strictly as string data and is never used
 * directly as a format string. This prevents format string attacks such
 * as those using %x or %n.
 *
 * @param device_name Pointer to the null-terminated device name string.
 *
 * @return None.
 */
static void log_device_status(const char *device_name)
{
    char status_message[128];
    int result;

    if (device_name == NULL)
    {
        return;
    }

    result = snprintf(status_message,
                      sizeof(status_message),
                      "Device online: %s",
                      device_name);

    if ((result < 0) || ((size_t)result >= sizeof(status_message)))
    {
        return;
    }

    (void)printf("%s\n", status_message);
}

/**
 * @brief Program entry point.
 *
 * Tests the logging function with a simulated malicious device name
 * containing printf format specifiers.
 *
 * @return EXIT_SUCCESS on successful execution.
 */
int main(void)
{
    const char *attacker_payload = "Sensor_%x_%x_%x_%x";

    (void)printf("=== Exercise 2: Format String Patch ===\n");

    log_device_status(attacker_payload);

    return EXIT_SUCCESS;
}