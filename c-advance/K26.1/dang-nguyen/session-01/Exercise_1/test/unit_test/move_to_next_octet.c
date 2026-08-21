#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "ipv4_parser_test_private.h"
#include "ut_common.h"
#include "move_to_next_octet.h"

static uint32_t run_one_move_to_next_octet_test(st_move_to_next_octet_test_case_t *const p_test_case, const uint32_t test_idx);
static bool is_move_to_next_octet_test_passed(const st_move_to_next_octet_test_case_t *const p_test_case);
static void print_move_to_next_octet_pass(const st_move_to_next_octet_test_case_t *const p_test_case, const uint32_t test_idx);
static void print_move_to_next_octet_fail(const st_move_to_next_octet_test_case_t *const p_test_case, const uint32_t test_idx);

static st_move_to_next_octet_test_case_t s_move_tests[] =
{
    /* idx */ /* p_input */ /* null pp */ /* actual rem */ /* expected rem */ /* check rem */ /* actual_result */ /* expected_result */      /* p_description */
    { 0U,     ".168.1.1",  false,        NULL,           "168.1.1",       true,          EC_RET_SUCCESS,     EC_RET_SUCCESS,            "Consume delimiter after first octet"       },
    { 1U,     ".1.1",      false,        NULL,           "1.1",           true,          EC_RET_SUCCESS,     EC_RET_SUCCESS,            "Consume delimiter after second octet"      },
    { 2U,     ".1",        false,        NULL,           "1",             true,          EC_RET_SUCCESS,     EC_RET_SUCCESS,            "Consume delimiter after third octet"       },
    { 3U,     "",          false,        NULL,           "",              true,          EC_RET_SUCCESS,     EC_RET_SUCCESS,            "Do not consume delimiter after last octet" },
    { 0U,     "",          false,        NULL,           "",              true,          EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject missing next octet"                },
    { 0U,     "a.1.1",     false,        NULL,           "a.1.1",         true,          EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject invalid delimiter"                 },
    { 4U,     ".1",        false,        NULL,           ".1",            true,          EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject invalid octet index"               },
    { 0U,     ".1",        true,         NULL,           ".1",            true,          EC_RET_SUCCESS,     EC_RET_ARG_NULL,           "Reject NULL pp_str"                       },
    { 0U,     NULL,        false,        NULL,           NULL,            true,          EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      "Reject NULL current string pointer"       }
};

uint32_t get_move_to_next_octet_test_count(void)
{
    return ARRAY_SIZE(s_move_tests);
}

static bool is_move_to_next_octet_test_passed(const st_move_to_next_octet_test_case_t *const p_test_case)
{
    bool is_result_matched = false;
    bool is_remaining_matched = true;

    is_result_matched = (p_test_case->actual_result == p_test_case->expected_result);

    if (false != p_test_case->check_remaining)
    {
        is_remaining_matched = is_string_matched(
            p_test_case->p_actual_remaining,
            p_test_case->p_expected_remaining);
    }

    return ((false != is_result_matched) &&
            (false != is_remaining_matched));
}

static void print_move_to_next_octet_pass(const st_move_to_next_octet_test_case_t *const p_test_case, const uint32_t test_idx)
{
    printf("Unit test case %u: [PASS] function = move_to_next_octet, input = %s, remaining = %s, return = %s, description: %s\n",
        test_idx,
        input_to_string(p_test_case->p_input),
        input_to_string(p_test_case->p_actual_remaining),
        errcode_to_string(p_test_case->actual_result),
        p_test_case->p_description);
}

static void print_move_to_next_octet_fail(const st_move_to_next_octet_test_case_t *const p_test_case, const uint32_t test_idx)
{
    printf("Unit test case %u: [FAIL] function = move_to_next_octet, input = %s",
        test_idx,
        input_to_string(p_test_case->p_input));

    if ((false != p_test_case->check_remaining) &&
        (false == is_string_matched(p_test_case->p_actual_remaining, p_test_case->p_expected_remaining)))
    {
        printf(", actual remaining = %s (expected remaining = %s)",
            input_to_string(p_test_case->p_actual_remaining),
            input_to_string(p_test_case->p_expected_remaining));
    }
    else
    {
        printf(", actual remaining = %s",
            input_to_string(p_test_case->p_actual_remaining));
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

static uint32_t run_one_move_to_next_octet_test(st_move_to_next_octet_test_case_t *const p_test_case, const uint32_t test_idx)
{
    const char *p_cursor = NULL;
    const char **pp_str = NULL;
    uint32_t fail_count = 0U;

    if (NULL == p_test_case)
    {
        printf("Unit test case %u: [FAIL] function = move_to_next_octet, input = NULL, actual return = EC_RET_UNKNOWN, description: test case pointer is NULL\n",
            test_idx);
        fail_count = 1U;
    }
    else
    {
        p_cursor = p_test_case->p_input;
        pp_str = &p_cursor;

        if (false != p_test_case->use_null_pp_str)
        {
            pp_str = NULL;
        }

        p_test_case->actual_result = move_to_next_octet(p_test_case->idx, pp_str);
        p_test_case->p_actual_remaining = p_cursor;

        if (false != is_move_to_next_octet_test_passed(p_test_case))
        {
            print_move_to_next_octet_pass(p_test_case, test_idx);
        }
        else
        {
            print_move_to_next_octet_fail(p_test_case, test_idx);
            fail_count = 1U;
        }
    }

    return fail_count;
}

uint32_t run_move_to_next_octet_tests(uint32_t *const p_test_idx)
{
    uint32_t fail_count = 0U;

    if (NULL == p_test_idx)
    {
        printf("Unit test runner: [FAIL] move_to_next_octet test index pointer is NULL\n");
        fail_count = 1U;
    }
    else
    {
        for (uint32_t idx = 0U; idx < ARRAY_SIZE(s_move_tests); idx++)
        {
            fail_count += run_one_move_to_next_octet_test(&s_move_tests[idx], *p_test_idx);
            (*p_test_idx)++;
        }
    }

    return fail_count;
}