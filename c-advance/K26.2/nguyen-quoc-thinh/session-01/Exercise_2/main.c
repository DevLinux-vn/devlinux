/**
 * @file main.c
 * @brief Safe MAC address string parser demonstration.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define PARSE_SUCCESS ((int8_t)0)
#define PARSE_ERROR   ((int8_t)-1)
#define MAC_OCTET_COUNT UINT8_C(6)

/**
 * @brief Convert one hexadecimal character to its numeric value.
 *
 * @param[in] character Character to convert.
 * @param[out] p_value Pointer receiving the value from 0 to 15.
 * @return true if character is hexadecimal; otherwise false.
 */
static bool hex_value(char character, uint8_t *p_value)
{
    bool valid = false;

    if (NULL == p_value)
    {
        return false;
    }

    if ((character >= '0') && (character <= '9'))
    {
        *p_value = (uint8_t)(character - '0');
        valid = true;
    }
    else if ((character >= 'a') && (character <= 'f'))
    {
        *p_value = (uint8_t)((character - 'a') + 10);
        valid = true;
    }
    else if ((character >= 'A') && (character <= 'F'))
    {
        *p_value = (uint8_t)((character - 'A') + 10);
        valid = true;
    }
    else
    {
        valid = false;
    }

    return valid;
}

/**
 * @brief Parse a MAC address into six bytes.
 *
 * Accepts either colon or hyphen separators, consistently throughout
 * the address (for example, "00:1A:2B:3C:4D:5E").
 *
 * @param[in] mac_str Null-terminated MAC address string.
 * @param[out] p_mac_out Pointer to an output array of at least six bytes.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out)
{
    const char *p_current = mac_str;
    uint8_t octet_index = UINT8_C(0);
    char delimiter = '\0';
    int8_t result = PARSE_ERROR;
    uint8_t high_nibble = UINT8_C(0);
    uint8_t low_nibble = UINT8_C(0);

    if ((NULL == mac_str) || (NULL == p_mac_out))
    {
        return PARSE_ERROR;
    }

    while (UINT8_C(6) > octet_index)
    {
        if (('\0' == p_current[0]) || ('\0' == p_current[1]))
        {
            return PARSE_ERROR;
        }

        if (false == hex_value(p_current[0], &high_nibble))
        {
            return PARSE_ERROR;
        }
        if (false == hex_value(p_current[1], &low_nibble))
        {
            return PARSE_ERROR;
        }

        p_mac_out[octet_index] =
            (uint8_t)(((uint16_t)high_nibble << 4U) | (uint16_t)low_nibble);

        p_current += 2;
        octet_index++;

        if (MAC_OCTET_COUNT > octet_index)
        {
            char current_delimiter = *p_current;

            if (('\0' == current_delimiter) ||
                ((':' != current_delimiter) && ('-' != current_delimiter)))
            {
                return PARSE_ERROR;
            }

            if ('\0' == delimiter)
            {
                delimiter = current_delimiter;
            }
            else if (delimiter != current_delimiter)
            {
                return PARSE_ERROR;
            }

            p_current++;
        }
    }

    if ('\0' != *p_current)
    {
        return PARSE_ERROR;
    }

    result = PARSE_SUCCESS;
    return result;
}

int main(void)
{
    uint8_t mac_out[MAC_OCTET_COUNT] = {UINT8_C(0)};
    int8_t parse_result = PARSE_ERROR;
    uint8_t index = UINT8_C(0);

    parse_result = parse_mac("00:1A:2B:3C:4D:5E", mac_out);
    if (PARSE_SUCCESS == parse_result)
    {
        (void)printf("parse_mac result: %d, MAC = ", (int)parse_result);
        for (index = UINT8_C(0); index < MAC_OCTET_COUNT; index++)
        {
            (void)printf("%02X%s", (unsigned int)mac_out[index],
                         (index < (MAC_OCTET_COUNT - UINT8_C(1))) ? ":" : "\n");
        }
    }
    else
    {
        (void)printf("MAC parse failed\n");
    }

    parse_result = parse_mac("00-1a-2b-3c-4d-5e", mac_out);
    (void)printf("parse_mac(hyphen form) -> %d\n", (int)parse_result);

    parse_result = parse_mac("00:1A:2B:3C:4D:5G", mac_out);
    (void)printf("parse_mac(invalid hex) -> %d\n", (int)parse_result);

    parse_result = parse_mac(NULL, mac_out);
    (void)printf("parse_mac(NULL) -> %d\n", (int)parse_result);

    return 0;
}
