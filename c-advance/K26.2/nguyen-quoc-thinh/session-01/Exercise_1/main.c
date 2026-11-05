/**
 * @file main.c
 * @brief Safe IPv4 string parser demonstration.
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define PARSE_SUCCESS ((int8_t)0)
#define PARSE_ERROR   ((int8_t)-1)

/**
 * @brief Parse a decimal IPv4 address into a 32-bit unsigned integer.
 *
 * The first octet occupies the most significant byte. For example,
 * "192.168.1.50" becomes 0xC0A80132.
 *
 * @param[in] ip_str Null-terminated IPv4 address string.
 * @param[out] p_ip_out Pointer to the output integer.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out)
{
    uint32_t ip_value = UINT32_C(0);
    uint32_t octet_value = UINT32_C(0);
    uint8_t octet_count = UINT8_C(0);
    uint8_t digit_count = UINT8_C(0);
    const char *p_current = ip_str;
    int8_t result = PARSE_ERROR;

    if ((NULL == ip_str) || (NULL == p_ip_out))
    {
        return PARSE_ERROR;
    }

    while ('\0' != *p_current)
    {
        char current_char = *p_current;

        if ((current_char >= '0') && (current_char <= '9'))
        {
            uint32_t digit = (uint32_t)(current_char - '0');

            /* Prevent overflow/range violations before multiplication. */
            if (octet_value > UINT32_C(25))
            {
                return PARSE_ERROR;
            }
            if ((octet_value == UINT32_C(25)) && (digit > UINT32_C(5)))
            {
                return PARSE_ERROR;
            }

            octet_value = (octet_value * UINT32_C(10)) + digit;
            digit_count++;
            if (digit_count > UINT8_C(3))
            {
                return PARSE_ERROR;
            }
        }
        else if ('.' == current_char)
        {
            if ((UINT8_C(0) == digit_count) ||
                (octet_count >= UINT8_C(3)))
            {
                return PARSE_ERROR;
            }

            ip_value = (ip_value << 8U) | octet_value;
            octet_count++;
            octet_value = UINT32_C(0);
            digit_count = UINT8_C(0);
        }
        else
        {
            return PARSE_ERROR;
        }

        p_current++;
    }

    if ((UINT8_C(0) == digit_count) || (UINT8_C(3) != octet_count))
    {
        return PARSE_ERROR;
    }

    ip_value = (ip_value << 8U) | octet_value;
    *p_ip_out = ip_value;
    result = PARSE_SUCCESS;

    return result;
}

int main(void)
{
    uint32_t ip_out = UINT32_C(0);
    int8_t parse_result = PARSE_ERROR;

    parse_result = parse_ipv4("192.168.1.50", &ip_out);
    if (PARSE_SUCCESS == parse_result)
    {
        (void)printf("parse_ipv4(\"192.168.1.50\") -> %d, 0x%08" PRIX32
                     " (%" PRIu32 ")\n",
                     (int)parse_result, ip_out, ip_out);
    }
    else
    {
        (void)printf("IPv4 parse failed\n");
    }

    parse_result = parse_ipv4("256.0.0.1", &ip_out);
    (void)printf("parse_ipv4(\"256.0.0.1\") -> %d\n", (int)parse_result);

    parse_result = parse_ipv4(NULL, &ip_out);
    (void)printf("parse_ipv4(NULL) -> %d\n", (int)parse_result);

    return 0;
}
