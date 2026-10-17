#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define IPV4_OK                    ((int8_t)0)
#define IPV4_ERROR                 ((int8_t)-1)

#define IPV4_OCTET_COUNT           (4U)
#define IPV4_LAST_OCTET_INDEX      (3U)
#define IPV4_MAX_OCTET_VALUE       (255U)
#define IPV4_MAX_DIGITS_PER_OCTET  (3U)
#define IPV4_DECIMAL_BASE          (10U)
#define IPV4_OCTET_BITS            (8U)
#define IPV4_INPUT_BUFFER_SIZE     (32U)

/**
 * @brief Check whether a character is a decimal digit.
 *
 * @param[in] character Character to check.
 * @return true if character is a digit, false otherwise.
 */
static bool is_digit(char character)
{
    bool result = false;

    if ((character >= '0') && (character <= '9'))
    {
        result = true;
    }

    return result;
}

/**
 * @brief Parse one IPv4 octet.
 *
 * @param[in,out] p_p_current Pointer to the current string position.
 * @param[out]    p_octet_out Pointer to store the parsed octet.
 * @return 0 on success, -1 on invalid input.
 */
static int8_t parse_octet(const char **p_p_current, uint32_t *p_octet_out)
{
    uint32_t octet = 0U;
    uint8_t digit_count = 0U;
    int8_t status = IPV4_ERROR;

    while (is_digit(**p_p_current) == true)
    {
        uint32_t digit = (uint32_t)(**p_p_current - '0');

        if (digit_count >= IPV4_MAX_DIGITS_PER_OCTET)
        {
            break;
        }

        if (octet > ((IPV4_MAX_OCTET_VALUE - digit) / IPV4_DECIMAL_BASE))
        {
            break;
        }

        octet = (octet * IPV4_DECIMAL_BASE) + digit;
        digit_count++;
        (*p_p_current)++;
    }

    if (digit_count > 0U)
    {
        *p_octet_out = octet;
        status = IPV4_OK;
    }

    return status;
}

/**
 * @brief Parse an IPv4 address string into a 32-bit unsigned integer.
 *
 * @param[in]  ip_str    Null-terminated ASCII string.
 * @param[out] p_ip_out  Pointer to store the parsed 32-bit IP address.
 * @return 0 on success, -1 on invalid input.
 */
static int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out)
{
    const char *p_current = ip_str;
    uint32_t ip_value = 0U;
    uint8_t octet_index = 0U;
    int8_t status = IPV4_ERROR;

    if ((ip_str != NULL) && (p_ip_out != NULL))
    {
        status = IPV4_OK;

        for (octet_index = 0U; octet_index < IPV4_OCTET_COUNT; octet_index++)
        {
            uint32_t octet = 0U;

            if (parse_octet(&p_current, &octet) != IPV4_OK)
            {
                status = IPV4_ERROR;
                break;
            }

            ip_value = (ip_value << IPV4_OCTET_BITS) | octet;

            if (octet_index < IPV4_LAST_OCTET_INDEX)
            {
                if (*p_current == '.')
                {
                    p_current++;
                }
                else
                {
                    status = IPV4_ERROR;
                    break;
                }
            }
        }

        if ((status == IPV4_OK) && (*p_current == '\0'))
        {
            *p_ip_out = ip_value;
        }
        else
        {
            status = IPV4_ERROR;
        }
    }

    return status;
}

/**
 * @brief Remove the newline character from an input string.
 *
 * @param[in,out] p_text    Input string.
 * @param[in]     text_size Size of the input buffer.
 */
static void remove_newline(char *p_text, size_t text_size)
{
    size_t index = 0U;

    while (index < text_size)
    {
        if (p_text[index] == '\n')
        {
            p_text[index] = '\0';
            break;
        }

        if (p_text[index] == '\0')
        {
            break;
        }

        index++;
    }
}

int main(void)
{
    char ip_input[IPV4_INPUT_BUFFER_SIZE] = { 0 };
    uint32_t ip_out = 0U;
    int exit_code = 1;

    printf("=====================================\n");
    printf(" Safe IPv4 Address Parser\n");
    printf("=====================================\n");
    printf("Please enter an IPv4 address: ");

    if (fgets(ip_input, sizeof(ip_input), stdin) != NULL)
    {
        int8_t result = IPV4_ERROR;

        remove_newline(ip_input, sizeof(ip_input));

        result = parse_ipv4(ip_input, &ip_out);

        printf("\n");

        if (result == IPV4_OK)
        {
            printf("Result      : Valid IPv4 address\n");
            printf("Input       : %s\n", ip_input);
            printf("Return code : %d\n", (int)result);
            printf("Hex value   : 0x%08" PRIX32 "\n", ip_out);
            printf("Decimal     : %" PRIu32 "\n", ip_out);

            exit_code = 0;
        }
        else
        {
            printf("Result      : Invalid IPv4 address\n");
            printf("Input       : %s\n", ip_input);
            printf("Return code : %d\n", (int)result);
            printf("Reason      : IPv4 address must have exactly four numbers\n");
            printf("              from 0 to 255 separated by dots.\n");
            printf("Example     : 192.168.1.50\n");

            exit_code = 1;
        }
    }
    else
    {
        printf("\nInput error.\n");
        exit_code = 1;
    }

    return exit_code;
}