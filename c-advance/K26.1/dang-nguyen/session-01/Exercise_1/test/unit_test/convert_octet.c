#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "ipv4_parser_test_private.h"
#include "ut_common.h"
#include "convert_octet.h"

static uint32_t run_one_convert_octet_test(st_convert_octet_test_case_t *const p_test_case, const uint32_t test_idx);
static bool is_convert_octet_test_passed(const st_convert_octet_test_case_t *const p_test_case);
static void print_convert_octet_pass(const st_convert_octet_test_case_t *const p_test_case, const uint32_t test_idx);
static void print_convert_octet_fail(const st_convert_octet_test_case_t *const p_test_case, const uint32_t test_idx);

static st_convert_octet_test_case_t s_convert_tests[] =
{
    /* p_input */    /* null pp */ /* null octet */ /* actual_octet */ /* expected_octet */ /* check octet */ /* actual rem */ /* expected rem */ /* check rem */ /* actual_result */ /* expected_result */      /* p_description */
    { "192.168",     false,        false,           0U,                192U,               true,             NULL,           ".168",          true,          EC_RET_SUCCESS,     EC_RET_SUCCESS,            "Convert normal octet"                  },
    { "0.0.0",       false,        false,           0U,                0U,                 true,             NULL,           ".0.0",          true,          EC_RET_SUCCESS,     EC_RET_SUCCESS,            "Convert lower boundary octet"          },
    { "255.",        false,        false,           0U,                255U,               true,             NULL,           ".",             true,          EC_RET_SUCCESS,     EC_RET_SUCCESS,            "Convert upper boundary octet"          },
    { "256.",        false,        false,           0U,                0U,                 false,            NULL,           "256.",          true,          EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject octet value greater than 255"   },
    { "1000.",       false,        false,           0U,                0U,                 false,            NULL,           "1000.",         true,          EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject octet with more than 3 digits"  },
    { ".168",        false,        false,           0U,                0U,                 true,             NULL,           ".168",          true,          EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject empty octet"                    },
    { NULL,          false,        false,           0U,                0U,                 true,             NULL,           NULL,            true,          EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject NULL string pointer value"      },
    { "192",         true,         false,           0U,                0U,                 true,             NULL,           "192",           true,          EC_RET_SUCCESS,     EC_RET_ARG_NULL,           "Reject NULL pp_str"                    },
    { "192",         false,        true,            0U,                0U,                 true,             NULL,           "192",           true,          EC_RET_SUCCESS,     EC_RET_ARG_NULL,           "Reject NULL p_octet"                   }
};

uint32_t get_convert_octet_test_count(void)
{
    return ARRAY_SIZE(s_convert_tests);
}

static bool is_convert_octet_test_passed(const st_convert_octet_test_case_t *const p_test_case)
{
    bool is_result_matched = false;
    bool is_octet_matched = true;
    bool is_remaining_matched = true;

    is_result_matched = (p_test_case->actual_result == p_test_case->expected_result);

    if (false != p_test_case->check_octet)
    {
        is_octet_matched = (p_test_case->actual_octet == p_test_case->expected_octet);
    }

    if (false != p_test_case->check_remaining)
    {
        is_remaining_matched = is_string_matched(
            p_test_case->p_actual_remaining,
            p_test_case->p_expected_remaining);
    }

    return ((false != is_result_matched) &&
            (false != is_octet_matched) &&
            (false != is_remaining_matched));
}

static void print_convert_octet_pass(const st_convert_octet_test_case_t *const p_test_case, const uint32_t test_idx)
{
    printf("Unit test case %u: [PASS] function = convert_octet, input = %s, output = %u, remaining = %s, return = %s, description: %s\n",
        test_idx,
        input_to_string(p_test_case->p_input),
        p_test_case->actual_octet,
        input_to_string(p_test_case->p_actual_remaining),
        errcode_to_string(p_test_case->actual_result),
        p_test_case->p_description);
}

static void print_convert_octet_fail(const st_convert_octet_test_case_t *const p_test_case, const uint32_t test_idx)
{
    printf("Unit test case %u: [FAIL] function = convert_octet, input = %s",
        test_idx,
        input_to_string(p_test_case->p_input));

    if ((false != p_test_case->check_octet) &&
        (p_test_case->actual_octet != p_test_case->expected_octet))
    {
        printf(", actual output = %u (expected = %u)",
            p_test_case->actual_octet,
            p_test_case->expected_octet);
    }
    else
    {
        printf(", actual output = %u", p_test_case->actual_octet);
    }

    if ((false != p_test_case->check_remaining) &&
        (false == is_string_matched(p_test_case->p_actual_remaining, p_test_case->p_expected_remaining)))
    {
        printf(", actual remaining = %s (expected remaining = %s)",
            input_to_string(p_test_case->p_actual_remaining),
            input_to_string(p_test_case->p_expected_remaining));
    }

    printf(", actual return = %s",
        errcode_to_string(p_test_case->actual_result));

    if (p_test_case->actual_result != p_test_case->expected_result)
    {
        printf(" (expected return = %s)",
            errcode_to_string(p_test_case->expected_result));
    }

    printf(", description: %s\n", p_test_case->p_description);
}

static uint32_t run_one_convert_octet_test(st_convert_octet_test_case_t *const p_test_case, const uint32_t test_idx)
{
    const char *p_cursor = NULL;
    const char **pp_str = NULL;
    uint32_t octet = 0U;
    uint32_t *p_octet = NULL;
    uint32_t fail_count = 0U;

    if (NULL == p_test_case)
    {
        printf("Unit test case %u: [FAIL] function = convert_octet, input = NULL, actual return = EC_RET_UNKNOWN, description: test case pointer is NULL\n",
            test_idx);
        fail_count = 1U;
    }
    else
    {
        p_cursor = p_test_case->p_input;
        pp_str = &p_cursor;
        p_octet = &octet;

        if (false != p_test_case->use_null_pp_str)
        {
            pp_str = NULL;
        }

        if (false != p_test_case->use_null_octet)
        {
            p_octet = NULL;
        }

        p_test_case->actual_result = convert_octet(pp_str, p_octet);
        p_test_case->actual_octet = octet;
        p_test_case->p_actual_remaining = p_cursor;

        if (false != is_convert_octet_test_passed(p_test_case))
        {
            print_convert_octet_pass(p_test_case, test_idx);
        }
        else
        {
            print_convert_octet_fail(p_test_case, test_idx);
            fail_count = 1U;
        }
    }

    return fail_count;
}

uint32_t run_convert_octet_tests(uint32_t *const p_test_idx)
{
    uint32_t fail_count = 0U;

    if (NULL == p_test_idx)
    {
        printf("Unit test runner: [FAIL] convert_octet test index pointer is NULL\n");
        fail_count = 1U;
    }
    else
    {
        for (uint32_t idx = 0U; idx < ARRAY_SIZE(s_convert_tests); idx++)
        {
            fail_count += run_one_convert_octet_test(&s_convert_tests[idx], *p_test_idx);
            (*p_test_idx)++;
        }
    }

    return fail_count;
}