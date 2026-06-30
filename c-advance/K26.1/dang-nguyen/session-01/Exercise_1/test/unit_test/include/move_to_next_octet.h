#ifndef MOVE_TO_NEXT_OCTET_H
#define MOVE_TO_NEXT_OCTET_H

#include <stdbool.h>
#include <stdint.h>
#include "ipv4_parser.h"

typedef struct
{
    uint8_t     idx;
    const char  *p_input;
    bool        use_null_pp_str;

    const char  *p_actual_remaining;
    const char  *p_expected_remaining;
    bool        check_remaining;

    e_errcode_t actual_result;
    e_errcode_t expected_result;
    const char  *p_description;
} st_move_to_next_octet_test_case_t;

uint32_t get_move_to_next_octet_test_count(void);
uint32_t run_move_to_next_octet_tests(uint32_t *const p_test_idx);

#endif  /* MOVE_TO_NEXT_OCTET_H */