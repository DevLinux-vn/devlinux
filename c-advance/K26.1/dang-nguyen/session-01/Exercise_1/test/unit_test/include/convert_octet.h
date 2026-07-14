#ifndef CONVERT_OCTET_H
#define CONVERT_OCTET_H

#include <stdbool.h>
#include <stdint.h>
#include "ipv4_parser.h"

typedef struct
{
    const char  *p_input;
    bool        use_null_pp_str;
    bool        use_null_octet;

    uint32_t    actual_octet;
    uint32_t    expected_octet;
    bool        check_octet;

    const char  *p_actual_remaining;
    const char  *p_expected_remaining;
    bool        check_remaining;

    e_errcode_t actual_result;
    e_errcode_t expected_result;
    const char  *p_description;
} st_convert_octet_test_case_t;

uint32_t get_convert_octet_test_count(void);
uint32_t run_convert_octet_tests(uint32_t *const p_test_idx);

#endif  /* CONVERT_OCTET_H */