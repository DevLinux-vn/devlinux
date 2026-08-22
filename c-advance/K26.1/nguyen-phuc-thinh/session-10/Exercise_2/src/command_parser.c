/**
 * @file command_parser.c
 * @brief Implementation of the command parser module.
 */
#include "command_parser.h"
#include <string.h>
#include <stdio.h>

int8_t extract_device_name(const char *p_cmd, char *p_device, size_t device_size) 
{
    if ((p_cmd == NULL) || (p_device == NULL) || (device_size == 0U)) 
    {
        return -1;
    }

    const char *p_colon = strchr(p_cmd, ':');
    if (p_colon == NULL) 
    {
        return -1;
    }

    /* Calculate length using pointer arithmetic */
    size_t name_len = (size_t)(p_colon - p_cmd);
    
    /* Ensure we do not overflow the destination buffer (Leave room for '\0') */
    if (name_len >= device_size) 
    {
        name_len = device_size - 1U;
    }

    /* Manually copy the substring and null-terminate safely */
    for (size_t i = 0U; i < name_len; i++) 
    {
        p_device[i] = p_cmd[i];
    }
    p_device[name_len] = '\0';

    return 0;
}

int8_t parse_command(const char *p_cmd, char *p_response, size_t resp_size) 
{
    if ((p_cmd == NULL) || (p_response == NULL) || (resp_size == 0U)) 
    {
        return -1;
    }

    char device[32];
    if (extract_device_name(p_cmd, device, sizeof(device)) != 0) 
    {
        snprintf(p_response, resp_size, "[ERROR] Invalid format");
        return -1;
    }

    const char *p_payload = strchr(p_cmd, ':');
    if (p_payload == NULL) 
    {
        return -1;
    }
    p_payload++; /* Move past the colon to the payload section */

    if (strcmp(device, "LED") == 0) 
    {
        if (strcmp(p_payload, "ON") == 0) 
        {
            snprintf(p_response, resp_size, "[OK] LED set to ON");
        } 
        else if (strcmp(p_payload, "OFF") == 0) 
        {
            snprintf(p_response, resp_size, "[OK] LED set to OFF");
        } 
        else 
        {
            snprintf(p_response, resp_size, "[ERROR] Unknown LED command");
            return -1;
        }
    } 
    else if (strcmp(device, "MOTOR") == 0) 
    {
        if (strncmp(p_payload, "START:", 6) == 0) 
        {
            int rpm = 0;
            /* Safely parse the RPM value; check sscanf return value (ERR33-C) */
            if (sscanf(p_payload + 6, "%d", &rpm) == 1) 
            {
                snprintf(p_response, resp_size, "[OK] MOTOR started at %d RPM", rpm);
            } 
            else 
            {
                snprintf(p_response, resp_size, "[ERROR] Invalid RPM");
                return -1;
            }
        } 
        else if (strcmp(p_payload, "STOP") == 0) 
        {
            snprintf(p_response, resp_size, "[OK] MOTOR stopped");
        } 
        else 
        {
            snprintf(p_response, resp_size, "[ERROR] Unknown MOTOR command");
            return -1;
        }
    } 
    else if (strcmp(device, "TEMP") == 0) 
    {
        if (strcmp(p_payload, "READ") == 0) 
        {
            snprintf(p_response, resp_size, "[OK] TEMP read requested");
        } 
        else 
        {
            snprintf(p_response, resp_size, "[ERROR] Unknown TEMP command");
            return -1;
        }
    } 
    else 
    {
        snprintf(p_response, resp_size, "[ERROR] Unknown device");
        return -1;
    }

    return 0;
}