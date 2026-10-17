#ifndef MAC_PARSER_H
#define MAC_PARSER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Parse a MAC address string into a 6-byte array.
 *
 * @param[in]  mac_str    Null-terminated ASCII string (e.g., "00:1A:2B:3C:4D:5E").
 * @param[out] p_mac_out  Pointer to a 6-byte array to store the parsed MAC address.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out);

/**
 * @brief Convert a single hexadecimal character into a value in the range 0-15.
 *
 * @param[in]  c    @param[in] c Hexadecimal character ('0'-'9', 'A'-'F', or 'a'-'f').
 * @return Value in the range 0 - 15, or HEX_VALID if the character is invalid.      
 */
uint8_t hex_char_to_value(char c);

/**
 * @brief Check whether the number of parsed MAC address groups is valid.
 *
 * @param[in]  group    Numbers of parsed groups
 * @retval  true  The group count is within the valid range.
 * @retval  false The group count exceeds the valid range.
 */
bool is_valid_group_count(uint8_t group);

#endif