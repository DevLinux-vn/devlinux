#ifndef IPV4_PARSER_TEST_PRIVATE_H
#define IPV4_PARSER_TEST_PRIVATE_H

#include <stdint.h>
#include "ipv4_parser.h"

e_errcode_t validate_ip_boundary(const char *const p_ip_str);
e_errcode_t convert_octet(const char **pp_str, uint32_t *const p_octet);
e_errcode_t move_to_next_octet(const uint8_t idx, const char **pp_str);

#endif  /* IPV4_PARSER_TEST_PRIVATE_H */