#ifndef PARSE_IPV4_H
#define PARSE_IPV4_H

#include <stdint.h>

int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out);

#endif
