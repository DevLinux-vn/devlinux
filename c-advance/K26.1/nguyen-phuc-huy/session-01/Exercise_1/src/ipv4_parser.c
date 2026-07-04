#include "ipv4_parser.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Check if a character is a decimal digit.
 *
 * @param[in] current_char Input character
 * @return true if '0'–'9', otherwise false
 */
static bool is_digit(char current_char)
{
    return (CHAR_TO_DEC(current_char) >= 0);
}

/**
 * @brief Parse IPv4 string into uint32_t (A.B.C.D format)
 */
int8_t parse_ipv4(const char *p_ip_str, uint32_t *p_ip_out)
{
    if ((p_ip_str == NULL) || (p_ip_out == NULL))
    {
        return -1;
    }

    uint8_t dot_count = 0U;
    uint32_t octet = 0U;
    uint8_t octet_len = 0U;

    *p_ip_out = 0U;

    while (*p_ip_str != '\0')
    {
        char current_char = *p_ip_str;

        /* =========================
         * Handle dot separator
         * ========================= */
        if (current_char == DOT_ASCII)
        {
            /* reject empty octet (e.g. "1..2") */
            if (octet_len == 0U)
            {
                return -1;
            }

            dot_count++;

            if (dot_count > MAX_NUMBER_OF_DOT)
            {
                return -1;
            }

            *p_ip_out = (*p_ip_out << IPV4_SHIFT_OCTET_BIT) | octet;

            octet = 0U;
            octet_len = 0U;

            p_ip_str++;
            continue;
        }

        /* =========================
         * Validate digit
         * ========================= */
        if (!is_digit(current_char))
        {
            return -1;
        }

        octet = (octet * SHIFT_DECIMAL) + (uint32_t)(current_char - '0');

        if (octet > MAX_IPV4_OCTET_VALUE)
        {
            return -1;
        }

        octet_len++;
        p_ip_str++;
    }

    /* =========================
     * Final octet validation
     * ========================= */
    if (octet_len == 0U)
    {
        return -1;
    }

    if (dot_count != MAX_NUMBER_OF_DOT)
    {
        return -1;
    }

    *p_ip_out = (*p_ip_out << IPV4_SHIFT_OCTET_BIT) | octet;

    return 0;
}