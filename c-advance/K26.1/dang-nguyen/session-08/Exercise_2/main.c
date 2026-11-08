#include <stdio.h>
#include <string.h>

#define MSG_LEN     (128)

/**
 * @brief Prints an online status message for a device.
 *
 * Builds a status message using the supplied device name and writes it to
 * standard output. When compiled with VULNERABLE defined, the message is
 * passed directly as the format string; otherwise, it is printed as data.
 *
 * @param p_device_name Pointer to a null-terminated device name.
 */
void log_device_status(const char *p_device_name);

void log_device_status(const char *p_device_name)
{
    char status_message[MSG_LEN];
    (void)snprintf(status_message, sizeof(status_message), "Device online: %s", p_device_name);

#if defined(VULNERABLE)
    /* ⚠️ DANGER: Format String Vulnerability in the Processing Step! */
    printf(status_message);
#else
    printf("%s", status_message);
#endif

    printf("\n");
}

/**
 * @brief Runs the device status logging example.
 *
 * Sends a simulated network payload to log_device_status() and then exits.
 *
 * @return 0 when the program completes normally.
 */
int main(void)
{
    /* Simulated malicious input from network */
    const char *p_attacker_payload = "Sensor_%x_%x_%x_%x"; 
    log_device_status(p_attacker_payload);
    return 0;
}