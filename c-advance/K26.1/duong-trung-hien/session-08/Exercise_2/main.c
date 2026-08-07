#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define APP_SUCCESS (0)

/**
 * @brief Logs the status of a device safely.
 *
 * Creates a formatted status message containing the specified device
 * name and prints it to the console. The message is constructed using
 * snprintf() to prevent buffer overflow, and is printed using a fixed
 * format string to avoid format string vulnerabilities.
 *
 * @param device_name Pointer to a null-terminated string containing the
 *                    device name to be logged.
 */
void log_device_status(const char* device_name) {
    char untrusted_buffer[128];
    snprintf(untrusted_buffer, sizeof(untrusted_buffer), "Device online: %s", device_name);

    printf("%s", untrusted_buffer);
    printf("\n");
}

int32_t main(void) {
    printf("=== Exercise 2: Format String Patch ===\n");
    const char* data_payload = "Sensor_%x_%x_%x_%x";
    log_device_status(data_payload);

    return APP_SUCCESS;
}