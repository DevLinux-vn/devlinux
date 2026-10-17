#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* =========================
 * Application return codes
 * ========================= */
/**
 * @brief Success return code
 */
#define APP_SUCCESS   (0)

/**
 * @brief Failure return code
 */
#define APP_FAILURE   (-1)

/**
 * @brief Function success return code
 */
#define FUNC_SUCCESS  (0)

/**
 * @brief Function failure return code
 */
#define FUNC_FAILURE  (-1)

/**
 * @brief Number of bytes in MAC address
 */
#define MAC_LENGTH    (6U)

/**
 * @brief Number of bits to shift for high nibble
 */
#define NIBBLE_SHIFT  (4U)

/**
 * @brief Decimal base for hex conversion
 */
#define HEX_BASE_10   (10U)

/* =========================
 * HEX utilities
 * ========================= */

/**
 * @brief Check if character is a valid hexadecimal digit
 *
 * @param[in] character Input character
 * @return true if character is 0-9, A-F, or a-f
 */
static bool is_hex_char(char character)
{
    return ((character >= '0' && character <= '9') ||
            (character >= 'A' && character <= 'F') ||
            (character >= 'a' && character <= 'f'));
}

/**
 * @brief Convert hex character to numeric value (0–15)
 *
 * @param[in] character Input hex character
 * @param[out] p_value Output numeric value
 *
 * @return FUNC_SUCCESS on success
 * @return FUNC_FAILURE on invalid input
 */
static int8_t hex_to_value(char character, uint8_t *p_value)
{
    if ((p_value == NULL) || (!is_hex_char(character)))
    {
        return FUNC_FAILURE;
    }

    if (character >= '0' && character <= '9')
    {
        *p_value = (uint8_t)(character - '0');
    }
    else if (character >= 'A' && character <= 'F')
    {
        *p_value = (uint8_t)(character - 'A' + HEX_BASE_10);
    }
    else
    {
        *p_value = (uint8_t)(character - 'a' + HEX_BASE_10);
    }

    return FUNC_SUCCESS;
}

/* =========================
 * MAC parser
 * ========================= */

/**
 * @brief Parse MAC address string into 6-byte array
 *
 * Supported formats:
 * - "00:1A:2B:3C:4D:5E"
 * - "00-1a-2b-3c-4d-5e"
 *
 * @param[in]  p_mac_str Input MAC string
 * @param[out] p_mac_out Output 6-byte MAC array
 *
 * @return FUNC_SUCCESS on success
 * @return FUNC_FAILURE on invalid input or format error
 */
int8_t parse_mac(const char *p_mac_str, uint8_t *p_mac_out)
{
    if ((p_mac_str == NULL) || (p_mac_out == NULL))
    {
        return FUNC_FAILURE;
    }

    uint8_t byte_index = 0U;
    uint8_t high_nibble = 1U;

    for (uint8_t i = 0U; i < MAC_LENGTH; i++)
    {
        p_mac_out[i] = 0U;
    }

    while (*p_mac_str != '\0')
    {
        if (byte_index >= MAC_LENGTH)
        {
            return FUNC_FAILURE;
        }

        if (*p_mac_str == ':' || *p_mac_str == '-')
        {
            if (high_nibble == 0U)
            {
                return FUNC_FAILURE;
            }
        }
        else
        {
            uint8_t value = 0U;

            if (hex_to_value(*p_mac_str, &value) != FUNC_SUCCESS)
            {
                return FUNC_FAILURE;
            }

            if (high_nibble == 1U)
            {
                p_mac_out[byte_index] = (uint8_t)(value << NIBBLE_SHIFT);
                high_nibble = 0U;
            }
            else
            {
                p_mac_out[byte_index] |= value;
                high_nibble = 1U;
                byte_index++;
            }
        }

        p_mac_str++;
    }

    if ((byte_index != MAC_LENGTH) || (high_nibble != 1U))
    {
        return FUNC_FAILURE;
    }

    return FUNC_SUCCESS;
}

/* =========================
 * MAIN test program
 * ========================= */

/**
 * @brief Test entry point for MAC parser
 *
 * Runs predefined test cases and prints parsed results.
 *
 * @return APP_SUCCESS if all tests pass
 * @return APP_FAILURE if any test fails
 */
int32_t main(void)
{
    const char *p_mac[] =
    {
        "00:1A:2B:3C:4D:5E",
        "00-1a-2b-3c-4d-5e",
        "00:1A:2B:3C:4D",
        "00:1A:2B:3C:4D:5E:6F",
        "00:1A:2B:3C:4D:5G"
    };

    uint8_t mac_out[MAC_LENGTH] = {0U};
    int32_t exit_code = APP_SUCCESS;

    for (uint32_t i = 0U; i < (sizeof(p_mac) / sizeof(p_mac[0])); i++)
    {
        int8_t ret = parse_mac(p_mac[i], mac_out);

        if (ret != FUNC_SUCCESS)
        {
            printf("Invalid MAC address: %s\n", p_mac[i]);
            exit_code = APP_FAILURE;
        }
        else
        {
            printf("MAC address  : %s\n", p_mac[i]);
            printf("Parsed value : ");

            for (uint8_t j = 0U; j < MAC_LENGTH; j++)
            {
                printf("0x%02X", mac_out[j]);

                if (j < (MAC_LENGTH - 1U))
                {
                    printf(" ");
                }
            }
            printf("\n");
        }
    }

    return exit_code;
}