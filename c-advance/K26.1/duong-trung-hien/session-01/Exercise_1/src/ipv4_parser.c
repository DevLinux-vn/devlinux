#include "ipv4_parser.h"
#include <stddef.h>

int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out)
{
    if (ip_str == NULL || p_ip_out == NULL)
    {
        return -1;
    }

    uint32_t ip_out = 0U;
    uint32_t octet = 0U;
    uint8_t octet_count = 0U;

    bool has_digit = false;

    const char *p_current = ip_str;

    while (*p_current != '\0')
    {
        if ((*p_current >= '0') && (*p_current <= '9'))
        {
            has_digit = true;
            uint32_t digit = *p_current - '0';
            octet = (octet * 10U) + digit;

            if(octet > 255U)
            {
                return -1;
            }
        }
        else if (*p_current == '.')
        {
            if (!has_digit)
            {
                return -1;
            }

            if (octet_count >= 3U)
            {
                return -1;
            }

            uint8_t shift = (3U-octet_count) * 8U;

            ip_out |= (octet << shift);
            octet_count++;
            octet = 0;
            has_digit = false;

        }
        else
        {
            return -1;
        }

        p_current++;
    }

    if (!has_digit)
    {
        return -1;
    }

    uint8_t shift = (3U-octet_count) * 8U;
    ip_out |= (octet << shift);
    octet_count++;

    if(octet_count != 4U)
    {
        return -1;
    }

    *p_ip_out = ip_out;
    return 0;
}