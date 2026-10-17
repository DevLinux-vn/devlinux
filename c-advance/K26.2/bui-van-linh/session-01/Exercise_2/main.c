/**
 * @file main.c
 * @brief Safe MAC address parser (string -> uint8_t[6]) with self-tests.
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MAC_OCTETS        (6U)
#define MAC_HEX_DIGITS    (2U)
#define HEX_LETTER_OFFSET (10)
#define NIBBLE_BITS       (4U)
#define MAC_OK            ((int8_t)0)
#define MAC_ERROR         ((int8_t)-1)

/**
 * @brief Convert one hex character to its numeric value.
 *
 * @param[in]  ch        Character to convert ('0'-'9', 'a'-'f', 'A'-'F').
 * @param[out] p_nibble  Receives the value (0-15) when ch is valid.
 * @return true if ch is a hex digit, false otherwise.
 */
static bool hex_to_nibble(char ch, uint8_t *p_nibble)
{
    bool is_valid = true;

    if ((ch >= '0') && (ch <= '9'))
    {
        *p_nibble = (uint8_t)(ch - '0');
    }
    else if ((ch >= 'a') && (ch <= 'f'))
    {
        *p_nibble = (uint8_t)((ch - 'a') + HEX_LETTER_OFFSET);
    }
    else if ((ch >= 'A') && (ch <= 'F'))
    {
        *p_nibble = (uint8_t)((ch - 'A') + HEX_LETTER_OFFSET);
    }
    else
    {
        is_valid = false;
    }

    return is_valid;
}

/**
 * @brief Parse a MAC address string into a 6-byte array.
 *
 * Accepted formats: "AA:BB:CC:DD:EE:FF" or "AA-BB-CC-DD-EE-FF"
 * (one delimiter type per address, upper or lower case hex).
 * p_mac_out is only modified when parsing succeeds.
 *
 * @param[in]  mac_str    Null-terminated ASCII MAC string.
 * @param[out] p_mac_out  Pointer to a buffer of at least 6 bytes.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out)
{
    int8_t result = MAC_ERROR;
    bool valid = ((mac_str != NULL) && (p_mac_out != NULL));
    uint8_t mac_tmp[MAC_OCTETS] = {0U};
    uint8_t octet_idx = 0U;
    char delimiter = '\0';
    const char *p_cur = mac_str;

    while (valid && (octet_idx < MAC_OCTETS))
    {
        uint8_t high = 0U;
        uint8_t low = 0U;

        /* p_cur[1] is only read after p_cur[0] was a valid hex digit,
         * so it can never be beyond the terminating '\0'. */
        valid = hex_to_nibble(p_cur[0], &high);
        if (valid)
        {
            valid = hex_to_nibble(p_cur[1], &low);
        }

        if (valid)
        {
            mac_tmp[octet_idx] = (uint8_t)((uint8_t)(high << NIBBLE_BITS) | low);
            p_cur += MAC_HEX_DIGITS;
            octet_idx++;

            if (octet_idx < MAC_OCTETS)
            {
                if (((*p_cur == ':') || (*p_cur == '-')) &&
                    ((delimiter == '\0') || (delimiter == *p_cur)))
                {
                    delimiter = *p_cur;
                    p_cur++;
                }
                else
                {
                    valid = false;
                }
            }
        }
    }

    /* Nothing may follow the 6th octet. */
    if (valid && (*p_cur != '\0'))
    {
        valid = false;
    }

    if (valid)
    {
        uint8_t idx;

        for (idx = 0U; idx < MAC_OCTETS; idx++)
        {
            p_mac_out[idx] = mac_tmp[idx];
        }
        result = MAC_OK;
    }

    return result;
}

/* ---------------------------- self-tests ---------------------------- */

typedef struct
{
    const char *p_input;
    int8_t expected_ret;
    uint8_t expected_mac[MAC_OCTETS];
} test_case_t;

static const test_case_t g_tests[] =
{
    {"00:1A:2B:3C:4D:5E",    0, {0x00U, 0x1AU, 0x2BU, 0x3CU, 0x4DU, 0x5EU}},
    {"00-1a-2b-3c-4d-5e",    0, {0x00U, 0x1AU, 0x2BU, 0x3CU, 0x4DU, 0x5EU}},
    {"A2:F4:C1:AA:DF:AB",    0, {0xA2U, 0xF4U, 0xC1U, 0xAAU, 0xDFU, 0xABU}},
    {"00:1A:2B:3C:4D",      -1, {0U}},
    {"00:1A:2B:3C:4D:5E:6F",-1, {0U}},
    {"00:1A:2B:3C:4D:5G",   -1, {0U}},
    {"00:1A-2B:3C-4D:5E",   -1, {0U}},
    {"0:1A:2B:3C:4D:5E",    -1, {0U}},
    {"00:1A:2B:3C:4D:5E:",  -1, {0U}},
    {"",                    -1, {0U}}
};

#define NUM_TESTS (sizeof(g_tests) / sizeof(g_tests[0]))

/**
 * @brief Run one test case.
 * @param[in] p_test  Test case to run.
 * @return true if the test passed.
 */
static bool run_test(const test_case_t *p_test)
{
    uint8_t mac_out[MAC_OCTETS] = {0U};
    bool passed;
    uint8_t idx;
    int8_t ret = parse_mac(p_test->p_input, mac_out);

    passed = (ret == p_test->expected_ret);
    if (passed && (ret == MAC_OK))
    {
        for (idx = 0U; idx < MAC_OCTETS; idx++)
        {
            if (mac_out[idx] != p_test->expected_mac[idx])
            {
                passed = false;
            }
        }
    }

    (void)printf("[%s] parse_mac(\"%s\") -> %d\n",
                 passed ? "PASS" : "FAIL", p_test->p_input, (int)ret);
    return passed;
}

int main(void)
{
    int exit_code = 0;
    size_t idx;
    uint8_t mac_out[MAC_OCTETS] = {0U};

    for (idx = 0U; idx < NUM_TESTS; idx++)
    {
        if (!run_test(&g_tests[idx]))
        {
            exit_code = 1;
        }
    }

    if (parse_mac(NULL, mac_out) != MAC_ERROR)
    {
        (void)printf("[FAIL] NULL mac_str\n");
        exit_code = 1;
    }
    else
    {
        (void)printf("[PASS] NULL mac_str -> -1\n");
    }

    if (parse_mac("00:1A:2B:3C:4D:5E", NULL) != MAC_ERROR)
    {
        (void)printf("[FAIL] NULL p_mac_out\n");
        exit_code = 1;
    }
    else
    {
        (void)printf("[PASS] NULL p_mac_out -> -1\n");
    }

    return exit_code;
}