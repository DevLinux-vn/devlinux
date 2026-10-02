#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "ipv4_parser_test_private.h"
#include "ut_common.h"
#include "validate_ip_boundary.h"

static uint32_t run_one_validate_ip_boundary_test(st_validate_ip_boundary_test_case_t *const p_test_case, const uint32_t test_idx);

static st_validate_ip_boundary_test_case_t s_validate_tests[] =
{
    /* p_input */        /* actual_result */ /* expected_result */      /* p_description */
    { NULL,              EC_RET_SUCCESS,     EC_RET_ARG_NULL,           "Reject NULL IP string"                    },
    { "",                EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject empty IP string"                   },
    { ".192.168.1.50",   EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject IP string starting with non-digit" },
    { "a192.168.1.50",   EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject IP string starting with non-digit" },
    { "192.168.1.50.",   EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject IP string ending with non-digit"   },
    { "192.168.1.50a",   EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject IP string ending with non-digit"   },
    { "192.168.1.50",    EC_RET_SUCCESS,     EC_RET_SUCCESS,            "Accept valid boundary characters"         },
    { "192.168.2a.23",   EC_RET_SUCCESS,     EC_RET_SUCCESS,            "Accept boundary and leave full validation to parser" }
};

uint32_t get_validate_ip_boundary_test_count(void)
{
    return ARRAY_SIZE(s_validate_tests);
}

static uint32_t run_one_validate_ip_boundary_test(st_validate_ip_boundary_test_case_t *const p_test_case, const uint32_t test_idx)
{
    uint32_t fail_count = 0U;
    bool is_test_passed = false;

    if (NULL == p_test_case)
    {
        printf("Unit test case %u: [FAIL] function = validate_ip_boundary, input = NULL, actual return = EC_RET_UNKNOWN, description: test case pointer is NULL\n",
            test_idx);
        fail_count = 1U;
    }
    else
    {
        p_test_case->actual_result = validate_ip_boundary(p_test_case->p_input);
        is_test_passed = (p_test_case->actual_result == p_test_case->expected_result);

        if (false != is_test_passed)
        {
            printf("Unit test case %u: [PASS] function = validate_ip_boundary, input = %s, return = %s, description: %s\n",
                test_idx,
                input_to_string(p_test_case->p_input),
                errcode_to_string(p_test_case->actual_result),
                p_test_case->p_description);
        }
        else
        {
            printf("Unit test case %u: [FAIL] function = validate_ip_boundary, input = %s, actual return = %s (expected return = %s), description: %s\n",
                test_idx,
                input_to_string(p_test_case->p_input),
                errcode_to_string(p_test_case->actual_result),
                errcode_to_string(p_test_case->expected_result),
                p_test_case->p_description);

            fail_count = 1U;
        }
    }

    return fail_count;
}

uint32_t run_validate_ip_boundary_tests(uint32_t *const p_test_idx)
{
    uint32_t fail_count = 0U;

    if (NULL == p_test_idx)
    {
        printf("Unit test runner: [FAIL] validate_ip_boundary test index pointer is NULL\n");
        fail_count = 1U;
    }
    else
    {
        for (uint32_t idx = 0U; idx < ARRAY_SIZE(s_validate_tests); idx++)
        {
            fail_count += run_one_validate_ip_boundary_test(&s_validate_tests[idx], *p_test_idx);
            (*p_test_idx)++;
        }
    }

    return fail_count;
}