#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ut_common.h"

const char *errcode_to_string(const e_errcode_t ret)
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

const char *input_to_string(const char *const p_input)
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

bool is_string_matched(const char *const p_actual, const char *const p_expected)
{
    bool is_matched = false;

    if ((NULL == p_actual) && (NULL == p_expected))
    {
        is_matched = true;
    }
    else if ((NULL != p_actual) && (NULL != p_expected))
    {
        if (0 == strcmp(p_actual, p_expected))
        {
            is_matched = true;
        }
    }
    else
    {
        ;   /* Do nothing */
    }

    return is_matched;
}