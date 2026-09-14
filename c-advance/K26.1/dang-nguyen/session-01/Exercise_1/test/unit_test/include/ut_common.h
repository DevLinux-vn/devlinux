#ifndef UNIT_TEST_COMMON_H
#define UNIT_TEST_COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include "ipv4_parser.h"

#define ARRAY_SIZE(a)               (sizeof(a) / sizeof((a)[0]))

#define TEST_EXIT_SUCCESS           (0)
#define TEST_EXIT_FAILURE           (1)

#define OCTET_INVALID_INIT_VALUE    (0xA5A5A5A5U)

const char *errcode_to_string(const e_errcode_t ret);
const char *input_to_string(const char *const p_input);
bool is_string_matched(const char *const p_actual, const char *const p_expected);

#endif  /* UNIT_TEST_COMMON_H */