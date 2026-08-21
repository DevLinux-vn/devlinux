#ifndef VALIDATE_IP_BOUNDARY_H
#define VALIDATE_IP_BOUNDARY_H

#include <stdint.h>
#include "ipv4_parser.h"

typedef struct
{
    const char  *p_input;
    e_errcode_t actual_result;
    e_errcode_t expected_result;
    const char  *p_description;
} st_validate_ip_boundary_test_case_t;

uint32_t get_validate_ip_boundary_test_count(void);
uint32_t run_validate_ip_boundary_tests(uint32_t *const p_test_idx);

#endif  /* VALIDATE_IP_BOUNDARY_H */