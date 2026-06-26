#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define PARSE_OK        (0)
#define PARSE_ERROR     (-1)
#define MAC_BYTES       (6U)
#define HEX_BASE        (16U)

/**
 * @brief Convert a hexadecimal character to its numeric value.
 *
 * @param[in]  c            Hexadecimal character.
 * @param[out] p_hex_value  Pointer to store converted value.
 * @return true if character is valid hex, false otherwise.
 */
static bool hex_char_to_value(char c, uint8_t *p_hex_value)
{
    bool is_valid = true;

    if (p_hex_value == NULL)
    {
        is_valid = false;
    }
    else if ((c >= '0') && (c <= '9'))
    {
        *p_hex_value = (uint8_t)(c - '0');
    }
    else if ((c >= 'a') && (c <= 'f'))
    {
        *p_hex_value = (uint8_t)((c - 'a') + 10);
    }
    else if ((c >= 'A') && (c <= 'F'))
    {
        *p_hex_value = (uint8_t)((c - 'A') + 10);
    }
    else
    {
        is_valid = false;
    }

    return is_valid;
}

/**
 * @brief Parse a MAC address string into a 6-byte array.
 *
 * Examples:
 * "00:1A:2B:3C:4D:5E" -> {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E}
 * "00-1a-2b-3c-4d-5e" -> {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E}
 *
 * @param[in]  mac_str    Null-terminated MAC address string.
 * @param[out] p_mac_out  Pointer to 6-byte output array.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out)
{
    uint32_t byte_index = 0U;
    uint32_t pos = 0U;
    uint8_t high_nibble = 0U;
    uint8_t low_nibble = 0U;
    char delimiter = '\0';
    int8_t ret = PARSE_OK;

    if ((mac_str == NULL) || (p_mac_out == NULL))
    {
        ret = PARSE_ERROR;
    }
    else
    {
        for (byte_index = 0U; byte_index < MAC_BYTES; byte_index++)
        {
            if (hex_char_to_value(mac_str[pos], &high_nibble) == false)
            {
                ret = PARSE_ERROR;
                break;
            }

            pos++;

            if (hex_char_to_value(mac_str[pos], &low_nibble) == false)
            {
                ret = PARSE_ERROR;
                break;
            }

            pos++;

            p_mac_out[byte_index] = (uint8_t)(((uint32_t)high_nibble * HEX_BASE)
                                            + (uint32_t)low_nibble);

            if (byte_index < (MAC_BYTES - 1U))
            {
                if ((mac_str[pos] != ':') && (mac_str[pos] != '-'))
                {
                    ret = PARSE_ERROR;
                    break;
                }

                if (byte_index == 0U)
                {
                    delimiter = mac_str[pos];
                }
                else if (mac_str[pos] != delimiter)
                {
                    ret = PARSE_ERROR;
                    break;
                }
                else
                {
                    /* Delimiter is valid. */
                }

                pos++;
            }
            else
            {
                if (mac_str[pos] != '\0')
                {
                    ret = PARSE_ERROR;
                    break;
                }
            }
        }
    }

    return ret;
}

static void print_mac(const uint8_t *p_mac)
{
    uint32_t index = 0U;

    if (p_mac != NULL)
    {
        printf("{");

        for (index = 0U; index < MAC_BYTES; index++)
        {
            printf("0x%02X", p_mac[index]);

            if (index < (MAC_BYTES - 1U))
            {
                printf(", ");
            }
        }

        printf("}");
    }
}

int main(void)
{
    uint8_t mac_out[MAC_BYTES] = {0U, 0U, 0U, 0U, 0U, 0U};
    int8_t ret = PARSE_ERROR;

    ret = parse_mac("00:1A:2B:3C:4D:5E", mac_out);
    printf("ret = %d, mac_out = ", ret);
    print_mac(mac_out);
    printf("\n");

    ret = parse_mac("00-1a-2b-3c-4d-5e", mac_out);
    printf("ret = %d, mac_out = ", ret);
    print_mac(mac_out);
    printf("\n");

    ret = parse_mac("00:1A:2B:3C:4D", mac_out);
    printf("ret = %d\n", ret);

    ret = parse_mac("00:1A:2B:3C:4D:5E:6F", mac_out);
    printf("ret = %d\n", ret);

    ret = parse_mac("00:1A:2B:3C:4D:5G", mac_out);
    printf("ret = %d\n", ret);

    ret = parse_mac(NULL, mac_out);
    printf("ret = %d\n", ret);

    return 0;
}