/**
 * @file    command_parser.c
 * @brief   Implementation of the mini command parser.
 */
#include "command_parser.h"
#include <string.h>
#include <stdio.h>

int8_t extract_device_name(const char *p_cmd, char *p_device, size_t device_size)
{
    int8_t result = -1;

    /* MISRA Directive 4.11 / CERT EXP34-C: validate before use. */
    if ((p_cmd != NULL) && (p_device != NULL) && (device_size > 0U))
    {
        const char *p_colon = strchr(p_cmd, ':');

        if (p_colon != NULL)
        {
            size_t name_len = (size_t)(p_colon - p_cmd);

            /* CERT STR31-C: only copy if it (plus '\0') fits. */
            if (name_len < device_size)
            {
                (void)memcpy(p_device, p_cmd, name_len);
                p_device[name_len] = '\0';
                result = 0;
            }
        }
    }

    return result;
}

int8_t parse_command(const char *p_cmd, char *p_response, size_t resp_size)
{
    int8_t result = -1;

    if ((p_cmd != NULL) && (p_response != NULL) && (resp_size > 0U))
    {
        char device[16];

        if (extract_device_name(p_cmd, device, sizeof(device)) == 0)
        {
            /* extract_device_name() already proved a ':' exists. */
            const char *p_after_colon = strchr(p_cmd, ':') + 1;

            if (strcmp(device, "LED") == 0)
            {
                (void)snprintf(p_response, resp_size, "[OK] LED set to %s", p_after_colon);
                result = 0;
            }
            else if (strcmp(device, "MOTOR") == 0)
            {
                if (strncmp(p_after_colon, "START", 5U) == 0)
                {
                    const char *p_second_colon = strchr(p_after_colon, ':');

                    if (p_second_colon != NULL)
                    {
                        unsigned int rpm = 0U;

                        /* CERT ERR33-C: check sscanf's return value. */
                        if (sscanf(p_second_colon + 1, "%u", &rpm) == 1)
                        {
                            (void)snprintf(p_response, resp_size,
                                           "[OK] MOTOR started at %u RPM", rpm);
                            result = 0;
                        }
                        else
                        {
                            (void)snprintf(p_response, resp_size, "[ERR] Invalid RPM value");
                        }
                    }
                    else
                    {
                        (void)snprintf(p_response, resp_size, "[ERR] Missing RPM value");
                    }
                }
                else if (strcmp(p_after_colon, "STOP") == 0)
                {
                    (void)snprintf(p_response, resp_size, "[OK] MOTOR stopped");
                    result = 0;
                }
                else
                {
                    (void)snprintf(p_response, resp_size, "[ERR] Unknown MOTOR command");
                }
            }
            else if (strcmp(device, "TEMP") == 0)
            {
                if (strcmp(p_after_colon, "READ") == 0)
                {
                    (void)snprintf(p_response, resp_size, "[OK] TEMP read requested");
                    result = 0;
                }
                else
                {
                    (void)snprintf(p_response, resp_size, "[ERR] Unknown TEMP command");
                }
            }
            else
            {
                (void)snprintf(p_response, resp_size, "[ERR] Unknown device");
            }
        }
        else
        {
            (void)snprintf(p_response, resp_size, "[ERR] Invalid command format");
        }
    }

    return result;
}