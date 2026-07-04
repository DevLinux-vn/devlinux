#ifndef IPV4_PARSER_H
#define IPV4_PARSER_H

#include <stdint.h>   /**< For fixed-width integer types */

/* =========================

* ASCII constants
* ========================= */
  #define NULL_TERMINATOR              ('\0')
  #define DOT_ASCII                    ('.')

/* =========================

* IPv4 parsing constraints
* ========================= */
  #define MAX_NUMBER_OF_DOT            (3U)
  #define IPV4_SHIFT_OCTET_BIT         (8U)
  #define SHIFT_DECIMAL                (10U)
  #define MAX_IPV4_OCTET_VALUE         (255U)

/**

* @brief Parse IPv4 string into uint32_t
*
* @param[in]  p_ip_str   Pointer to IPv4 string (e.g., "192.168.1.1")
* @param[out] p_ip_out   Pointer to store result
*
* @return int8_t
* @retval  0  Success
* @retval -1  Invalid input or format error
  */
  int8_t parse_ipv4(const char *p_ip_str, uint32_t *p_ip_out);

#endif /* IPV4_PARSER_H */
