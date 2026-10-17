#include "command_parser.h"

int8_t parse_command(const char *p_cmd, char *p_response, size_t resp_size) {
    if (NULL == p_cmd || NULL == p_response) {
        return ERROR;
    }

    char device[resp_size];
    uint8_t result = extract_device_name(p_cmd, device, sizeof(device));
    if (result == FIND_DELIMITER_SUCCESS) {
        if (strcmp(device, "MOTOR") == STRING_SIMILAR) {
            uint32_t rpm;
            if (sscanf(p_cmd, "%*[^:]:%*[^:]:%u" , &rpm) == 1) {
                snprintf(p_response, resp_size, "[OK] %s started at %u RPM", device, rpm);
            }
            else 
            {
                snprintf(p_response, resp_size, "[OK] %s stopped", device);
            }
        }
        else if (strcmp(device, "LED") == STRING_SIMILAR) {
            const char *p_tmp = strchr(p_cmd, ':');
            snprintf(p_response, resp_size, "[OK] %s set to %s", device, p_tmp + 1);
        }
        else if (strcmp(device, "TEMP") == STRING_SIMILAR) {
            snprintf(p_response, resp_size, "[OK] %s read requested", device);
        }

        return PARSE_SUCCESS;
    }

    return PARSE_FAILED;
}

int8_t extract_device_name(const char *p_cmd, char *p_device, size_t device_size) {
    if (NULL == p_cmd || '\0' == p_cmd[0] || NULL == p_device) {
        return ERROR;
    }

    const char *p_tmp = strchr(p_cmd, ':');
    if (NULL == p_tmp) {
        return FIND_DELIMITER_FAILED;
    }

    size_t len = (size_t)(p_tmp - p_cmd);
    if (len <= device_size) {
        memcpy(p_device, p_cmd, len);
        p_device[len] = '\0';
        return FIND_DELIMITER_SUCCESS;
    }

    return FIND_DELIMITER_FAILED;
}