#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define APP_SUCCESS (0)

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