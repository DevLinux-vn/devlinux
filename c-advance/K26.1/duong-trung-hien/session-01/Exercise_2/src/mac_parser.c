#include "mac_parser.h"
#include <stddef.h>
#include <ctype.h>

int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out)
{
    if ((mac_str == NULL) || (p_mac_out == NULL))
    {
        return -1;
    }

    char separator = '\0';
    const char *p_current = mac_str;
    uint8_t group = 0;
    uint8_t token_index = 0;
    char token[3] = {0};

    while (*p_current != '\0')
    {
        //check hex
        if (isxdigit(*p_current))
        {
            if (token_index == 2)
                return -1;

            token[token_index] = *p_current;
            p_current++;
            token_index++;
        }
        else if ((*p_current == ':') || (*p_current == '-'))
        {
            if (separator == '\0')
                separator = *p_current;
            else if(*p_current != separator)
                return -1;

            if (token_index == 2)
            {
                token[2] = '\0';
                if(!is_valid_group_count(group))
                    return -1;

                uint8_t high = hex_char_to_value(token[0]);
                uint8_t low = hex_char_to_value(token[1]);
                p_mac_out[group] = (uint8_t)((high << 4U) | (low << 0));

                group++;
                token_index = 0;
                p_current++;

            }
            else
                return -1;
        }
        else
            return -1;

    }

    if (token_index == 2)
    {
        if(!is_valid_group_count(group))
            return -1;

        token[2] = '\0';

        uint8_t high = hex_char_to_value(token[0]);
        uint8_t low = hex_char_to_value(token[1]);
        p_mac_out[group] = (uint8_t)((high << 4U) | (low << 0));

        group++;
    }

            
    if (group != 6)
        return -1;

    return 0;
}

uint8_t hex_char_to_value(char c)
{
    if ((c >= '0') && (c <= '9'))
    {
        return (uint8_t)(c - '0');
    }

    if ((c >= 'A') && (c <= 'F'))
    {
        return (uint8_t)(c - 'A' + 10);
    }

    if ((c >= 'a') && (c <= 'f'))
    {
        return (uint8_t)(c - 'a' + 10);
    }
    
    return -1;
}

bool is_valid_group_count(uint8_t group)
{
    if (group > 5)
        return false;
    return true;
}