/**
 * @file main.c
 * @brief Safe Network Address Parser — MAC Address to uint8_t[6].
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define MAC_OCTET_COUNT (6U)

/**
 * @brief Parse a MAC address string into a 6-byte array.
 *
 * @param[in]  mac_str    Null-terminated ASCII string (e.g., "00:1A:2B:3C:4D:5E").
 * @param[out] p_mac_out  Pointer to a 6-byte array to store the parsed MAC address.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out) {
  if ((NULL == mac_str) || (NULL == p_mac_out)) {
    return -1;
  }

  uint8_t current_value = 0U;
  uint8_t octet_count = 0U;
  uint8_t digit_count = 0U;
  char delimiter = '\0';

  while (*mac_str != '\0') {
    if ((*mac_str >= '0') && (*mac_str <= '9')) {
      uint8_t digit = (uint8_t)(*mac_str - '0');
      current_value = (uint8_t)((current_value << 4U) | digit);
      digit_count++;
    } else if ((*mac_str >= 'A') && (*mac_str <= 'F')) {
      uint8_t digit = (uint8_t)((*mac_str - 'A') + 10);
      current_value = (uint8_t)((current_value << 4U) | digit);
      digit_count++;
    } else if ((*mac_str >= 'a') && (*mac_str <= 'f')) {
      uint8_t digit = (uint8_t)((*mac_str - 'a') + 10);
      current_value = (uint8_t)((current_value << 4U) | digit);
      digit_count++;
    } else if ((*mac_str == ':') || (*mac_str == '-')) {
      if ((2U != digit_count) || (octet_count >= 5U)) {
        return -1;
      }

      if ('\0' == delimiter) {
        delimiter = *mac_str;
      } else if (*mac_str != delimiter) {
        return -1;
      }

      p_mac_out[octet_count] = current_value;
      current_value = 0U;
      digit_count = 0U;
      octet_count++;
    } else {
      return -1;
    }

    if (digit_count > 2U) {
      return -1;
    }

    mac_str++;
  }

  if ((2U != digit_count) || (5U != octet_count)) {
    return -1;
  }

  p_mac_out[octet_count] = current_value;

  return 0;
}

int main(void) {
  uint8_t mac_out[MAC_OCTET_COUNT] = {0U};
  int8_t ret = 0;

  /* Test 1: Valid MAC with ':' delimiter */
  ret = parse_mac("00:1A:2B:3C:4D:5E", mac_out);
  (void)printf("parse_mac(\"00:1A:2B:3C:4D:5E\", mac_out)  -> returns %2d, "
               "mac_out = {0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, "
               "0x%02X}\n",
               ret, mac_out[0], mac_out[1], mac_out[2], mac_out[3], mac_out[4],
               mac_out[5]);

  /* Test 2: Valid MAC with '-' delimiter (lowercase) */
  ret = parse_mac("00-1a-2b-3c-4d-5e", mac_out);
  (void)printf("parse_mac(\"00-1a-2b-3c-4d-5e\", mac_out)  -> returns %2d, "
               "mac_out = {0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, "
               "0x%02X}\n",
               ret, mac_out[0], mac_out[1], mac_out[2], mac_out[3], mac_out[4],
               mac_out[5]);

  /* Test 3: Not enough octets */
  ret = parse_mac("00:1A:2B:3C:4D", mac_out);
  (void)printf("parse_mac(\"00:1A:2B:3C:4D\",    mac_out)  -> returns %2d  "
               "(Not enough octets)\n",
               ret);

  /* Test 4: Too many octets */
  ret = parse_mac("00:1A:2B:3C:4D:5E:6F", mac_out);
  (void)printf("parse_mac(\"00:1A:2B:3C:4D:5E:6F\", mac_out) -> returns %2d  "
               "(Too many octets)\n",
               ret);

  /* Test 5: Invalid hex character 'G' */
  ret = parse_mac("00:1A:2B:3C:4D:5G", mac_out);
  (void)printf("parse_mac(\"00:1A:2B:3C:4D:5G\", mac_out)  -> returns %2d  "
               "(Invalid hex character 'G')\n",
               ret);

  /* Test 6: NULL input */
  ret = parse_mac(NULL, mac_out);
  (void)printf("parse_mac(NULL,                 mac_out)  -> returns %2d  "
               "(Null input)\n",
               ret);

  return 0;
}