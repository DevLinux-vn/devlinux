#include "ipv4_parser.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Internal state used during IPv4 parsing.
 *
 * This structure stores intermediate parsing results for each octet,
 * dot counting, and leading-zero validation.
 */
typedef struct
{
    uint32_t octet;            /**< Current parsed octet value */
    uint8_t octet_length;      /**< Number of digits in current octet */
    uint8_t dot_count;         /**< Number of '.' encountered */
    char first_character;      /**< First character of current octet */
} ipv4_parse_state_t;

/**
 * @brief Check whether a character is a decimal digit.
 *
 * @param[in] current_char Input character to validate
 * @return true if character is between '0' and '9', otherwise false
 */
static bool is_digit(char current_char)
{
    return ((current_char >= '0') && (current_char <= '9'));
}

/**
 * @brief Process a digit character and update current octet state.
 *
 * This function builds the octet value incrementally and performs
 * overflow and range validation (0–255).
 *
 * @param[in] current_char Current digit character
 * @param[in,out] p_state Pointer to parser state structure
 *
 * @return int8_t
 * @retval 0  Success
 * @retval -1 Invalid digit or overflow detected
 */
static int8_t process_digit(char current_char, ipv4_parse_state_t *p_state)
{
    if (!is_digit(current_char))
    {
        return -1;
    }

    if (p_state->octet_length == 0U)
    {
        p_state->first_character = current_char;
    }

    if (p_state->octet > (MAX_IPV4_OCTET_VALUE / SHIFT_DECIMAL))
    {
        return -1;
    }

    p_state->octet = (p_state->octet * SHIFT_DECIMAL) +
                     (uint32_t)(current_char - '0');

    if (p_state->octet > MAX_IPV4_OCTET_VALUE)
    {
        return -1;
    }

    p_state->octet_length++;

    return 0;
}

/**
 * @brief Process dot separator and finalize current octet.
 *
 * This function validates the current octet before moving to the next one.
 *
 * @param[in,out] p_ip_out Pointer to final IPv4 result
 * @param[in,out] p_state  Pointer to parser state structure
 *
 * @return int8_t
 * @retval 0  Success
 * @retval -1 Invalid format (empty octet, leading zero, or too many dots)
 */
static int8_t process_dot(uint32_t *p_ip_out, ipv4_parse_state_t *p_state)
{
    if ((p_state->octet_length == 0U) ||
        ((p_state->octet_length > 1U) && (p_state->first_character == '0')))
    {
        return -1;
    }

    p_state->dot_count++;

    if (p_state->dot_count > MAX_NUMBER_OF_DOT)
    {
        return -1;
    }

    *p_ip_out = (*p_ip_out << IPV4_SHIFT_OCTET_BIT) | p_state->octet;

    p_state->octet = 0U;
    p_state->octet_length = 0U;

    return 0;
}

/**
 * @brief Parse IPv4 string into 32-bit integer representation.
 *
 * This function parses a dotted-decimal IPv4 string (e.g. "192.168.1.1")
 * into a 32-bit unsigned integer.
 *
 * Rules:
 * - Must contain exactly 4 octets
 * - Each octet must be in range 0–255
 * - Leading zeros are not allowed (e.g. "001" is invalid)
 *
 * @param[in]  p_ip_str  Pointer to null-terminated IPv4 string
 * @param[out] p_ip_out  Pointer to output 32-bit IPv4 value
 *
 * @return int8_t
 * @retval 0  Success
 * @retval -1 Invalid input or malformed IPv4 string
 */
int8_t parse_ipv4(const char *p_ip_str, uint32_t *p_ip_out)
{
    if ((p_ip_str == NULL) || (p_ip_out == NULL))
    {
        return -1;
    }

    ipv4_parse_state_t state = {0U, 0U, 0U, 0};

    *p_ip_out = 0U;

    while (*p_ip_str != NULL_TERMINATOR)
    {
        char current_char = *p_ip_str;

        if (current_char == DOT_ASCII)
        {
            if (process_dot(p_ip_out, &state) != 0)
            {
                return -1;
            }
        }
        else
        {
            if (process_digit(current_char, &state) != 0)
            {
                return -1;
            }
        }

        p_ip_str++;
    }

    /* Final validation */
    if ((state.octet_length == 0U) ||
        (state.dot_count != MAX_NUMBER_OF_DOT) ||
        ((state.octet_length > 1U) && (state.first_character == '0')))
    {
        return -1;
    }

    *p_ip_out = (*p_ip_out << IPV4_SHIFT_OCTET_BIT) | state.octet;

    return 0;
}