#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "ipv4_parser.h"

#define ARRAY_SIZE(a)               (sizeof(a) / sizeof((a)[0]))

#define TEST_EXIT_SUCCESS           (0)
#define TEST_EXIT_FAILURE           (1)

#define IPV4_INVALID_INIT_VALUE     (0xA5A5A5A5U)

typedef struct
{
    const char  *p_input;           /* Passed to p_ip_str of parse_ipv4(). */
    uint32_t    *p_output;          /* Passed to p_ip_out of parse_ipv4(). */

    e_errcode_t actual_result;      /* Actual return value of parse_ipv4(). */
    e_errcode_t expected_result;    /* Expected return value of parse_ipv4(). */

    uint32_t    expected_output;    /* Expected value stored in *p_output. */
    const char  *p_description;
} st_ipv4_test_case_t;

static const char *errcode_to_string(const e_errcode_t ret);
static const char *input_to_string(const char *const p_input);
static void print_ip_result(const uint32_t ip);
static void print_output_result(const uint32_t *const p_output);
static uint32_t run_one_test(st_ipv4_test_case_t *const p_test_case, const uint32_t test_idx);

static const char *errcode_to_string(const e_errcode_t ret)
{
    const char *p_ret_string = "EC_RET_UNKNOWN";

    switch (ret)
    {
        case EC_RET_SUCCESS:
            p_ret_string = "EC_RET_SUCCESS";
            break;

        case EC_RET_ARG_NULL:
            p_ret_string = "EC_RET_ARG_NULL";
            break;

        case EC_RET_INVALID_PARAM:
            p_ret_string = "EC_RET_INVALID_PARAM";
            break;

        default:
            p_ret_string = "EC_RET_UNKNOWN";
            break;
    }

    return p_ret_string;
}

static const char *input_to_string(const char *const p_input)
{
    const char *p_input_string = p_input;

    if (NULL == p_input)
    {
        p_input_string = "NULL";
    }
    else if ('\0' == p_input[0])
    {
        p_input_string = "\"\"";
    }
    else
    {
        ;   /* Do nothing */
    }

    return p_input_string;
}

static void print_ip_result(const uint32_t ip)
{
    printf("0x%08X (%u)", ip, ip);
}

static void print_output_result(const uint32_t *const p_output)
{
    if (NULL == p_output)
    {
        printf("NULL");
    }
    else
    {
        print_ip_result(*p_output);
    }
}

static uint32_t run_one_test(st_ipv4_test_case_t *const p_test_case, const uint32_t test_idx)
{
    uint32_t fail_count = 0U;
    bool is_result_matched = false;
    bool is_output_matched = false;
    bool is_test_passed = false;

    if (NULL == p_test_case)
    {
        printf("Test case %02u: [FAIL] input = NULL, actual output = NULL, actual return = EC_RET_UNKNOWN, description: test case pointer is NULL\n",
            test_idx);
        fail_count = 1U;
    }
    else
    {
        p_test_case->actual_result = parse_ipv4(p_test_case->p_input, p_test_case->p_output);

        is_result_matched = (p_test_case->actual_result == p_test_case->expected_result);

        if (NULL == p_test_case->p_output)
        {
            is_output_matched = true;
        }
        else
        {
            is_output_matched = (*(p_test_case->p_output) == p_test_case->expected_output);
        }

        is_test_passed = ((false != is_result_matched) && (false != is_output_matched));

        if (false != is_test_passed)
        {
            printf("Test case %02u: [PASS] input = %s, output = ",
                test_idx,
                input_to_string(p_test_case->p_input));

            print_output_result(p_test_case->p_output);

            printf(", return = %s, description: %s\n",
                errcode_to_string(p_test_case->actual_result),
                p_test_case->p_description);
        }
        else
        {
            printf("Test case %02u: [FAIL] input = %s, actual output = ",
                test_idx,
                input_to_string(p_test_case->p_input));

            print_output_result(p_test_case->p_output);

            if (false == is_output_matched)
            {
                printf(" (expected = ");
                print_ip_result(p_test_case->expected_output);
                printf(")");
            }

            printf(", actual return = %s",
                errcode_to_string(p_test_case->actual_result));

            if (false == is_result_matched)
            {
                printf(" (expected return = %s)",
                    errcode_to_string(p_test_case->expected_result));
            }

            printf(", description: %s\n", p_test_case->p_description);

            fail_count = 1U;
        }
    }

    return fail_count;
}

int32_t main(void)
{
    uint32_t ip_out_null_input      = IPV4_INVALID_INIT_VALUE;
    uint32_t ip_out_empty_string    = IPV4_INVALID_INIT_VALUE;
    uint32_t ip_out_over_255_last   = IPV4_INVALID_INIT_VALUE;
    uint32_t ip_out_over_255_first  = IPV4_INVALID_INIT_VALUE;
    uint32_t ip_out_missing_fourth  = IPV4_INVALID_INIT_VALUE;
    uint32_t ip_out_missing_third   = IPV4_INVALID_INIT_VALUE;
    uint32_t ip_out_empty_octet     = IPV4_INVALID_INIT_VALUE;
    uint32_t ip_out_extra_octet     = IPV4_INVALID_INIT_VALUE;
    uint32_t ip_out_invalid_char    = IPV4_INVALID_INIT_VALUE;
    uint32_t ip_out_valid_normal    = IPV4_INVALID_INIT_VALUE;
    uint32_t ip_out_zero            = IPV4_INVALID_INIT_VALUE;
    uint32_t ip_out_broadcast       = IPV4_INVALID_INIT_VALUE;

    st_ipv4_test_case_t s_test_cases[] =
    {
        /* p_input */         /* p_output */              /* actual_result */ /* expected_result */      /* expected_output */       /* p_description */
        { NULL,               &ip_out_null_input,         EC_RET_SUCCESS,     EC_RET_ARG_NULL,           IPV4_INVALID_INIT_VALUE,    "Reject NULL input string"             },
        { "",                 &ip_out_empty_string,       EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      IPV4_INVALID_INIT_VALUE,    "Reject empty string"                  },
        { "192.168.1.50",     NULL,                       EC_RET_SUCCESS,     EC_RET_ARG_NULL,           IPV4_INVALID_INIT_VALUE,    "Reject NULL output pointer"           },
        { "192.168.1.256",    &ip_out_over_255_last,      EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      IPV4_INVALID_INIT_VALUE,    "Reject octet value greater than 255"  },
        { "256.0.0.1",        &ip_out_over_255_first,     EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      IPV4_INVALID_INIT_VALUE,    "Reject first octet greater than 255"  },
        { "192.168.1",        &ip_out_missing_fourth,     EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      IPV4_INVALID_INIT_VALUE,    "Reject missing fourth octet"          },
        { "192.168.",         &ip_out_missing_third,      EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      IPV4_INVALID_INIT_VALUE,    "Reject missing third octet"           },
        { "192..168.1",       &ip_out_empty_octet,        EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      IPV4_INVALID_INIT_VALUE,    "Reject empty octet"                   },
        { "127.0.0.5.1",      &ip_out_extra_octet,        EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      IPV4_INVALID_INIT_VALUE,    "Reject extra octet"                   },
        { "192.168.2a.23",    &ip_out_invalid_char,       EC_RET_SUCCESS,     EC_RET_INVALID_PARAM,      IPV4_INVALID_INIT_VALUE,    "Reject invalid character in octet"    },
        { "192.168.1.50",     &ip_out_valid_normal,       EC_RET_SUCCESS,     EC_RET_SUCCESS,            0xC0A80132U,                "Valid normal IPv4 address"            },
        { "0.0.0.0",          &ip_out_zero,               EC_RET_SUCCESS,     EC_RET_SUCCESS,            0x00000000U,                "Valid lower boundary IPv4 address"    },
        { "255.255.255.255",  &ip_out_broadcast,          EC_RET_SUCCESS,     EC_RET_SUCCESS,            0xFFFFFFFFU,                "Valid upper boundary IPv4 address"    }
    };

    uint32_t total_tests = ARRAY_SIZE(s_test_cases);
    uint32_t fail_count = 0U;
    int32_t exit_code = TEST_EXIT_SUCCESS;

    for (uint32_t test_idx = 0U; test_idx < total_tests; test_idx++)
    {
        fail_count += run_one_test(&s_test_cases[test_idx], test_idx + 1U);
    }

    printf("\nTotal tests : %u\n", total_tests);
    printf("Failed      : %u\n", fail_count);

    if (0U != fail_count)
    {
        exit_code = TEST_EXIT_FAILURE;
    }

    return exit_code;
}