/**
 * @file main.c
 * @brief Secure Standard I/O — Format String Vulnerability Patch.
 * @details Demonstrates fixing a format string vulnerability (CWE-134 / CERT FIO30-C)
 *          by separating the format string literal ("%s") from untrusted input data.
 *          Complies with MISRA-C 2012 Directive 4.14 and BARR-C coding standards.
 * @author tran-doan-trung
 * @date 2026-11-28
 */

#include <stdio.h>
#include <string.h>

/**
 * @brief Safely format and print the status log for a given device name.
 * @details Uses "%s" format specifier so format tokens (e.g., %x, %n) in
 *          p_device_name are treated strictly as literal string data.
 * @param[in] p_device_name Null-terminated device name string. Must not be NULL.
 */
static void log_device_status(const char *p_device_name)
{
    if (NULL == p_device_name) {
        return;
    }

    char status_message[128] = { 0 };
    (void)snprintf(status_message, sizeof(status_message), "Device online: %s", p_device_name);

    /* Safe print: separate format string from untrusted buffer (CERT FIO30-C) */
    printf("%s\n", status_message);
}

/**
 * @brief Main entry point testing the patched logger against format string payload.
 * @return 0 on success.
 */
int main(void)
{
    /* Simulated malicious input from network */
    const char *p_attacker_payload = "Sensor_%x_%x_%x_%x";

    printf("=== Exercise 2: Format String Patch ===\n");
    log_device_status(p_attacker_payload);

    return 0;
}