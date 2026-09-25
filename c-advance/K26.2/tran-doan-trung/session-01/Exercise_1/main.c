/**
 * @file main.c
 * @brief Safe Network Address Parser — IPv4 to uint32_t.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Parse an IPv4 address string into a 32-bit unsigned integer.
 *
 * @param[in]  ip_str    Null-terminated ASCII string (e.g., "192.168.1.50").
 * @param[out] p_ip_out  Pointer to store the parsed 32-bit IP address.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out) {
  if ((NULL == ip_str) || (NULL == p_ip_out)) {
    return -1; /* NULL pointer */
  }

  uint32_t result = 0U;
  uint32_t current_value = 0U;
  uint8_t octet_count = 0U;
  uint8_t digit_count = 0U;

  while (*ip_str != '\0') {
    if ((*ip_str >= '0') && (*ip_str <= '9')) {
      uint32_t digit = (uint32_t)(*ip_str - '0');
      current_value = (current_value * 10U) + digit;

      if (current_value > 255U) {
        return -1; /* Octet out of range */
      }

      digit_count++;
    } else if (*ip_str == '.') {
      if ((0U == digit_count) || (octet_count >= 3U)) {
        return -1; /* Invalid format */
      }

      result = (result << 8U) | current_value;
      current_value = 0U;
      digit_count = 0U;
      octet_count++;
    } else {
      return -1; /* Invalid character */
    }

    ip_str++;
  }

  if ((0U == digit_count) || (3U != octet_count)) {
    return -1; /* Invalid format */
  }

  *p_ip_out = (result << 8U) | current_value;

  return 0; /* Success */
}

int main(void) {
  uint32_t ip_out = 0U;
  int8_t ret = 0;

  /* Test 1: Valid IP address */
  ret = parse_ipv4("192.168.1.50", &ip_out);
  (void)printf("parse_ipv4(\"192.168.1.50\", &ip_out)  -> returns %2d, ip_out "
               "= 0x%08X (%u)\n",
               ret, ip_out, ip_out);

  /* Test 2: Octet out of range */
  ret = parse_ipv4("256.0.0.1", &ip_out);
  (void)printf("parse_ipv4(\"256.0.0.1\", &ip_out)  -> returns %2d  (octet "
               "out of range)\n",
               ret);

  /* Test 3: NULL input */
  ret = parse_ipv4(NULL, &ip_out);
  (void)printf("parse_ipv4(NULL, &ip_out)  -> returns %2d  (NULL input)\n",
               ret);

  return 0;
}