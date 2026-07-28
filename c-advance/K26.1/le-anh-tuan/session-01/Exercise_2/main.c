#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum
{
    PARSE_MAC_OK = 0,
    PARSE_MAC_ERROR = -1,
    MAC_OCTET_COUNT = 6,
    MAC_LAST_OCTET_INDEX = 5,
    MAC_STRING_LENGTH = 17,
    MAC_GROUP_WIDTH = 3,
    MAC_LOW_NIBBLE_OFFSET = 1,
    MAC_SEPARATOR_OFFSET = 2,
    HEX_RADIX = 16,
    HEX_ALPHA_OFFSET = 10,
    INPUT_BUFFER_LENGTH = 64
};

/**
 * @brief Convert one hexadecimal ASCII character to its numeric value.
 *
 * @param[in]  input_character  ASCII character to convert.
 * @param[out] p_value_out      Pointer to the converted value.
 *
 * @return true if the character is a valid hex digit, otherwise false.
 */


static bool hex_char_to_value(const char input_character, uint8_t * const p_value_out)
{
    bool is_valid = false;

    if (p_value_out != NULL)
    {
        if ((input_character >= '0') && (input_character <= '9'))
        {
            *p_value_out = (uint8_t)(input_character - '0');
            is_valid = true;
        }
        else if ((input_character >= 'a') && (input_character <= 'f'))
        {
            *p_value_out = (uint8_t)((input_character - 'a') + HEX_ALPHA_OFFSET);
            is_valid = true;
        }
        else if ((input_character >= 'A') && (input_character <= 'F'))
        {
            *p_value_out = (uint8_t)((input_character - 'A') + HEX_ALPHA_OFFSET);
            is_valid = true;
        }
        else
        {
            is_valid = false;
        }
    }

    return is_valid;
}

/**
 * @brief Check whether a MAC delimiter is supported.
 *
 * @param[in] separator_character  Character to check.
 *
 * @return true if the delimiter is ':' or '-', otherwise false.
 */


static bool is_valid_separator(const char separator_character)
{
    bool is_valid = false;

    if ((separator_character == ':') || (separator_character == '-'))
    {
        is_valid = true;
    }

    return is_valid;
}

/**
 * @brief Check whether the input string length is exactly a MAC address length.
 *
 * @param[in] p_mac_str  Pointer to a null-terminated MAC address string.
 *
 * @return true if the string length is exactly 17 characters, otherwise false.
 */


static bool has_valid_mac_length(const char * const p_mac_str)
{
    bool is_valid = false;

    if (p_mac_str != NULL)
    {
        size_t character_index = 0U;

        while ((character_index <= (size_t)MAC_STRING_LENGTH) &&
               (p_mac_str[character_index] != '\0'))
        {
            character_index++;
        }

        if (character_index == (size_t)MAC_STRING_LENGTH)
        {
            is_valid = true;
        }
    }

    return is_valid;
}

/**
 * @brief Parse one two-digit hexadecimal MAC octet.
 *
 * @param[in]  p_mac_str     Pointer to the MAC address string.
 * @param[in]  base_index    Index of the first hexadecimal character.
 * @param[out] p_octet_out   Pointer to the parsed octet output.
 *
 * @return true if the octet is valid, otherwise false.
 */


static bool parse_mac_octet(const char * const p_mac_str,
                            const size_t base_index,
                            uint8_t * const p_octet_out)
{
    bool is_valid = false;

    if ((p_mac_str != NULL) && (p_octet_out != NULL))
    {
        uint8_t high_nibble = 0U;
        uint8_t low_nibble = 0U;

        if ((hex_char_to_value(p_mac_str[base_index], &high_nibble)) &&
            (hex_char_to_value(p_mac_str[base_index + MAC_LOW_NIBBLE_OFFSET],
                               &low_nibble)))
        {
            *p_octet_out = (uint8_t)((high_nibble * HEX_RADIX) + low_nibble);
            is_valid = true;
        }
    }

    return is_valid;
}

/**
 * @brief Check whether the separator after a MAC octet is valid.
 *
 * @param[in] p_mac_str             Pointer to the MAC address string.
 * @param[in] octet_index           Current MAC octet index.
 * @param[in] separator_character   Expected separator character.
 *
 * @return true if the separator position is valid, otherwise false.
 */


static bool has_valid_octet_separator(const char * const p_mac_str,
                                      const size_t base_index,
                                      const bool is_last_octet,
                                      const char separator_character)
{
    bool is_valid = false;

    if (p_mac_str != NULL)
    {
        is_valid = is_last_octet || (p_mac_str[base_index + MAC_SEPARATOR_OFFSET] == separator_character);
    }

    return is_valid;
}

/**
 * @brief Parse all six MAC address octets.
 *
 * @param[in]  p_mac_str             Pointer to the MAC address string.
 * @param[in]  separator_character   Expected separator character.
 * @param[out] p_mac_out             Pointer to a 6-byte output array.
 *
 * @return true if all octets are valid, otherwise false.
 */


static bool parse_all_mac_octets(const char * const p_mac_str,
                                 const char separator_character,
                                 uint8_t * const p_mac_out)
{
    bool is_valid = true;
    size_t octet_index = 0U;

    for (octet_index = 0U;
         (octet_index < (size_t)MAC_OCTET_COUNT) && is_valid;
         octet_index++)
    {
        const size_t base_index = octet_index * (size_t)MAC_GROUP_WIDTH;
        const bool is_last_octet = (octet_index == (size_t)MAC_LAST_OCTET_INDEX);

        if ((!parse_mac_octet(p_mac_str, base_index, &p_mac_out[octet_index])) ||
            (!has_valid_octet_separator(p_mac_str,
                                        base_index,
                                        is_last_octet,
                                        separator_character)))
        {
            is_valid = false;
        }
    }

    return is_valid;
}

/**
 * @brief Parse a MAC address string into a 6-byte array.
 *
 * @param[in]  p_mac_str  Null-terminated ASCII string.
 * @param[out] p_mac_out  Pointer to a 6-byte output array.
 *
 * @return 0 on success, negative value on failure.
 */


static int8_t parse_mac(const char * const p_mac_str, uint8_t * const p_mac_out)
{
    int8_t status = (int8_t)PARSE_MAC_ERROR;

    if ((p_mac_str != NULL) && (p_mac_out != NULL))
    {
        if (has_valid_mac_length(p_mac_str))
        {
            const char separator_character = p_mac_str[MAC_SEPARATOR_OFFSET];

            if (is_valid_separator(separator_character))
            {
                uint8_t parsed_mac[MAC_OCTET_COUNT] = {0U};

                if (parse_all_mac_octets(p_mac_str, separator_character, parsed_mac))
                {
                    size_t octet_index = 0U;

                    for (octet_index = 0U;
                         octet_index < (size_t)MAC_OCTET_COUNT;
                         octet_index++)
                    {
                        p_mac_out[octet_index] = parsed_mac[octet_index];
                    }

                    status = (int8_t)PARSE_MAC_OK;
                }
            }
        }
    }

    return status;
}

/**
 * @brief Replace the first newline character in a string with a null terminator.
 *
 * @param[in,out] p_input_buffer  User input buffer.
 */


static void remove_line_ending(char * const p_input_buffer)
{
    if (p_input_buffer != NULL)
    {
        size_t character_index = 0U;
        bool is_finished = false;

        while ((character_index < (size_t)INPUT_BUFFER_LENGTH) && (!is_finished))
        {
            if (p_input_buffer[character_index] == '\n')
            {
                p_input_buffer[character_index] = '\0';
                is_finished = true;
            }
            else if (p_input_buffer[character_index] == '\0')
            {
                is_finished = true;
            }
            else
            {
                character_index++;
            }
        }
    }
}

/**
 * @brief Print a parsed MAC address.
 *
 * @param[in] p_mac_out  Pointer to parsed MAC address array.
 */

 
static void print_mac_address(const uint8_t * const p_mac_out)
{
    if (p_mac_out != NULL)
    {
        size_t octet_index = 0U;

        printf("mac_out = {");

        for (octet_index = 0U;
             octet_index < (size_t)MAC_OCTET_COUNT;
             octet_index++)
        {
            printf("0x%02X", (unsigned int)p_mac_out[octet_index]);

            if (octet_index < (size_t)MAC_LAST_OCTET_INDEX)
            {
                printf(", ");
            }
        }

        printf("}\n");
    }
}

/**
 * @brief Program entry point.
 *
 * @return EXIT_SUCCESS if parsing succeeds, otherwise EXIT_FAILURE.
 */


int main(void)
{
    char input_buffer[INPUT_BUFFER_LENGTH] = {0};
    int exit_code = EXIT_FAILURE;

    printf("Enter MAC address: ");

    if (fgets(input_buffer, INPUT_BUFFER_LENGTH, stdin) != NULL)
    {
        uint8_t mac_out[MAC_OCTET_COUNT] = {0U};

        remove_line_ending(input_buffer);

        if (parse_mac(input_buffer, mac_out) == (int8_t)PARSE_MAC_OK)
        {
            printf("parse_mac() returned 0\n");
            print_mac_address(mac_out);
            exit_code = EXIT_SUCCESS;
        }
        else
        {
            printf("parse_mac() returned -1\n");
            printf("Invalid MAC address\n");
        }
    }
    else
    {
        printf("Input error\n");
    }

    return exit_code;
}