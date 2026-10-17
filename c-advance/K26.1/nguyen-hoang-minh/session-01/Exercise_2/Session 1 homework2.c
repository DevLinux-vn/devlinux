#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

/*
 * MISRA Directive 4.12: no malloc/free/realloc anywhere in this file —
 * every object below is a fixed-size, stack-allocated automatic variable.
 *
 * MISRA Rule 21.7 / CERT MSC24-C: no atoi(), atol(), atoll(), or sscanf()
 * anywhere in this file — every conversion below is hand-rolled,
 * character-by-character hex parsing so each partial result can be
 * validated before it is used.
 */

#define MAC_OCTET_COUNT          (6U)
#define MAC_HEX_DIGITS_PER_OCTET (2U)
#define MAC_NIBBLE_SHIFT_BITS    (4U)
#define HEX_ALPHA_DIGIT_OFFSET   (10U) /* value represented by 'a'/'A' */

/**
 * @brief Convert a single ASCII hex character into its 4-bit value.
 *
 * @param[in]  hex_char  Character to convert ('0'-'9', 'a'-'f', 'A'-'F').
 * @param[out] p_value   Parsed nibble value (0-15) on success.
 * @return true on success, false if hex_char is not a valid hex digit.
 */
static bool hex_char_to_value(char hex_char, uint8_t *p_value)
{
    bool result;

    /* MISRA Rule 9.1: initialize before it is read. */
    result = true;

    if ((hex_char >= '0') && (hex_char <= '9'))
    {
        /*
         * MISRA Rule 10.3 / CERT INT31-C: hex_char has already been range
         * checked against '0'-'9', so the subtraction result is 0-9 and
         * narrows to uint8_t without loss.
         */
        *p_value = (uint8_t)(hex_char - '0');
    }
    else if ((hex_char >= 'a') && (hex_char <= 'f'))
    {
        /*
         * CERT INT31-C / INT32-C: 'a'-'f' map to 10-15; hex_char is bounded
         * by the range check above, so (hex_char - 'a') is 0-5 and cannot
         * overflow before HEX_ALPHA_DIGIT_OFFSET is added — the narrowing
         * to uint8_t below is therefore safe.
         */
        *p_value = (uint8_t)((hex_char - 'a') + HEX_ALPHA_DIGIT_OFFSET);
    }
    else if ((hex_char >= 'A') && (hex_char <= 'F'))
    {
        /* Same reasoning as the 'a'-'f' branch above, for uppercase input. */
        *p_value = (uint8_t)((hex_char - 'A') + HEX_ALPHA_DIGIT_OFFSET);
    }
    else
    {
        /* MISRA Directive 4.14: reject any character that is not hex. */
        result = false;
    }

    return result;
}

/**
 * @brief Parse a two-character hex octet starting at *p_index within mac_str.
 *
 * @param[in]     mac_str  Null-terminated string being parsed.
 * @param[in,out] p_index  Index to start reading from; advanced past the
 *                         two hex digits consumed on success.
 * @param[out]    p_octet  Parsed byte value (0-255) on success.
 * @return true on success, false if the octet is malformed.
 */
static bool parse_hex_octet(const char *mac_str, size_t *p_index, uint8_t *p_octet)
{
    size_t  idx;
    uint8_t high_nibble;
    uint8_t low_nibble;
    bool    result;

    /* MISRA Rule 9.1: every automatic-storage local is set before it is read. */
    idx = *p_index;
    high_nibble = 0U;
    low_nibble = 0U;
    result = true;

    /*
     * ARR30-C / CERT ARR38-C: mac_str[idx] is checked for '\0' before any
     * further indexing, so mac_str[idx + 1U] is never read past a
     * terminator that this function itself has not already stopped at.
     *
     * NOLINTBEGIN(bugprone-branch-clone): each branch below intentionally
     * ends in the same `result = false;` body, but each guards a distinct
     * rejection reason (short string, bad high nibble, short string again,
     * bad low nibble). MISRA Rule 13.5 forbids folding these into a single
     * `&&` condition here, because hex_char_to_value() has a persistent
     * side effect (it writes through p_value) and the standard forbids
     * side effects inside the right-hand operand of a logical `&&`/`||`.
     * Keeping the guard clauses separate keeps every side effect in its
     * own fully-evaluated statement instead of inside a boolean operator.
     */
    if (mac_str[idx] == '\0')
    {
        result = false;
    }
    else if (!hex_char_to_value(mac_str[idx], &high_nibble))
    {
        result = false;
    }
    else if (mac_str[idx + 1U] == '\0')
    {
        result = false;
    }
    else if (!hex_char_to_value(mac_str[idx + 1U], &low_nibble))
    {
        result = false;
    }
    else
    {
        /*
         * MISRA Rule 10.3 / CERT INT31-C: both nibbles are already proven
         * to be in 0-15 by hex_char_to_value(), so the combined value
         * fits in uint8_t (0-255) without loss.
         *
         * CERT INT32-C: high_nibble is at most 0xF, so the shift result is
         * at most 0xF0 — well within uint8_t range — and ORing in
         * low_nibble (at most 0xF) cannot carry out of the byte, so no
         * arithmetic overflow is possible here.
         */
        *p_octet = (uint8_t)((high_nibble << MAC_NIBBLE_SHIFT_BITS) | low_nibble);
        *p_index = idx + MAC_HEX_DIGITS_PER_OCTET;
    }
    /* NOLINTEND(bugprone-branch-clone) */

    return result;
}

/**
 * @brief Parse a MAC address string into a 6-byte array.
 *
 * Declared with external linkage on purpose: this is the public API of the
 * module (see the required prototype), intended to be called from other
 * translation units via a shared header. In this single-file demo it is
 * only ever called from main() below, which is why static analyzers such
 * as cppcheck report it as "could be static" — that report does not apply
 * once this function is exposed through a shared header in a real project.
 *
 * @param[in]  mac_str    Null-terminated ASCII string (e.g., "00:1A:2B:3C:4D:5E").
 * @param[out] p_mac_out  Pointer to a 6-byte array to store the parsed MAC address.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out) // NOLINT(misc-use-internal-linkage)
{
    int8_t  status;
    size_t  index;
    uint8_t octet_count;
    uint8_t octet;
    char    delimiter;

    if ((mac_str == NULL) || (p_mac_out == NULL))
    {
        /*
         * MISRA Directive 4.11 / CERT EXP34-C: both pointer arguments are
         * validated before either is ever dereferenced.
         *
         * This early return is the one deliberate exception to MISRA
         * Rule 15.5 (single point of exit): it is a pure guard clause with
         * no cleanup to perform. Every other path below funnels through
         * the single `return status;` at the end.
         */
        return -1;
    }

    /* MISRA Rule 9.1: initialize all locals before they are read. */
    status = 0;
    index = 0U;
    octet_count = 0U;
    octet = 0U;
    delimiter = '\0';

    /*
     * MISRA Rule 14.2 targets *for* loops specifically (well-formed
     * init/condition/increment). This loop uses `while` instead because it
     * needs to `break` out early on a malformed octet or bad delimiter —
     * the loop-control intent Rule 14.2 protects (a single, clearly bounded
     * counter that is never modified inside the body except via the
     * increment) still holds: octet_count is only ever incremented once
     * per iteration, directly controls loop exit, and is never written
     * anywhere else.
     */
    while (octet_count < MAC_OCTET_COUNT)
    {
        if (!parse_hex_octet(mac_str, &index, &octet))
        {
            /* MISRA Directive 4.14: malformed octet rejected. */
            status = -1;
            break;
        }

        /*
         * ARR30-C / CERT ARR38-C: octet_count is bounded above by the
         * while condition (< MAC_OCTET_COUNT), so this write never goes
         * past p_mac_out[MAC_OCTET_COUNT - 1] — the caller's buffer is
         * never overflowed.
         */
        p_mac_out[octet_count] = octet;
        octet_count++;

        if (octet_count < MAC_OCTET_COUNT)
        {
            if ((mac_str[index] != ':') && (mac_str[index] != '-'))
            {
                /*
                 * MISRA Directive 4.14: a delimiter is mandatory between
                 * octets — untrusted input that omits it (or has too few
                 * octets) is rejected here.
                 */
                status = -1;
                break;
            }

            if (delimiter == '\0')
            {
                /* First delimiter seen fixes the expected style. */
                delimiter = mac_str[index];
            }
            else if (mac_str[index] != delimiter)
            {
                /*
                 * MISRA Directive 4.14: mixing ':' and '-' within one
                 * address (e.g. "00:1A-2B...") is untrusted-input
                 * malformation and is rejected here.
                 */
                status = -1;
                break;
            }
            else
            {
                /* Delimiter matches the established style; continue. */
            }

            index++;
        }
    }

    if (status == 0)
    {
        if (mac_str[index] != '\0')
        {
            /*
             * MISRA Directive 4.14: trailing characters after the 6th
             * octet (e.g. a 7th octet) make the whole address invalid.
             */
            status = -1;
        }
    }

    /* MISRA Rule 15.5: single point of exit for the validated-input path. */
    return status;
}

/*
 * CERT STR31-C: not applicable here — parse_mac() never copies mac_str
 * into a local buffer, so there is no fixed-size destination that could
 * overflow. All parsing reads directly from the caller-owned,
 * null-terminated string.
 */

/**
 * @brief Compare a parsed MAC address against an expected byte array.
 *
 * Kept as a helper (instead of six inline literal comparisons per test
 * case) so no single expected byte value appears more than once in this
 * file — each named expected-value array documents its own MAC address.
 *
 * @param[in] p_actual    Parsed MAC address to check.
 * @param[in] p_expected  Expected 6-byte MAC address.
 * @return true if all MAC_OCTET_COUNT bytes match.
 */
static bool mac_bytes_match(const uint8_t *p_actual, const uint8_t *p_expected)
{
    size_t octet_idx;
    bool   match;

    /* MISRA Rule 9.1: initialize before it is read. */
    match = true;

    for (octet_idx = 0U; octet_idx < MAC_OCTET_COUNT; octet_idx++)
    {
        if (p_actual[octet_idx] != p_expected[octet_idx])
        {
            match = false;
            break;
        }
    }

    return match;
}

/* Expected result shared by the ':' and '-' delimiter test cases below. */
static const uint8_t EXPECTED_MAC_STANDARD[MAC_OCTET_COUNT] =
{
    0x00U, 0x1AU, 0x2BU, 0x3CU, 0x4DU, 0x5EU
};

/**
 * @brief Simple test harness exercising the expected-output cases.
 *
 * @return 0 if all checks pass, non-zero if any check fails.
 */
int main(void)
{
    int8_t  parse_rc;
    uint8_t mac_out[MAC_OCTET_COUNT];
    int     exit_code;
    size_t  octet_idx;

    exit_code = 0;
    for (octet_idx = 0U; octet_idx < MAC_OCTET_COUNT; octet_idx++)
    {
        mac_out[octet_idx] = 0U;
    }

    /* CERT ERR33-C: parse_mac()'s return value is always checked below. */
    parse_rc = parse_mac("00:1A:2B:3C:4D:5E", mac_out);
    if ((parse_rc != 0) || !mac_bytes_match(mac_out, EXPECTED_MAC_STANDARD))
    {
        (void)printf("FAIL: \"00:1A:2B:3C:4D:5E\" parse_rc=%d\n", parse_rc);
        exit_code = 1;
    }
    else
    {
        (void)printf("PASS: \"00:1A:2B:3C:4D:5E\" -> parse_rc=%d\n", parse_rc);
    }

    parse_rc = parse_mac("00-1a-2b-3c-4d-5e", mac_out);
    if ((parse_rc != 0) || !mac_bytes_match(mac_out, EXPECTED_MAC_STANDARD))
    {
        (void)printf("FAIL: \"00-1a-2b-3c-4d-5e\" parse_rc=%d\n", parse_rc);
        exit_code = 1;
    }
    else
    {
        (void)printf("PASS: \"00-1a-2b-3c-4d-5e\" -> parse_rc=%d\n", parse_rc);
    }

    parse_rc = parse_mac("00:1A:2B:3C:4D", mac_out);
    if (parse_rc != -1)
    {
        (void)printf("FAIL: \"00:1A:2B:3C:4D\" parse_rc=%d (expected -1)\n", parse_rc);
        exit_code = 1;
    }
    else
    {
        (void)printf("PASS: \"00:1A:2B:3C:4D\" -> parse_rc=%d\n", parse_rc);
    }

    parse_rc = parse_mac("00:1A:2B:3C:4D:5E:6F", mac_out);
    if (parse_rc != -1)
    {
        (void)printf("FAIL: \"00:1A:2B:3C:4D:5E:6F\" parse_rc=%d (expected -1)\n", parse_rc);
        exit_code = 1;
    }
    else
    {
        (void)printf("PASS: \"00:1A:2B:3C:4D:5E:6F\" -> parse_rc=%d\n", parse_rc);
    }

    parse_rc = parse_mac("00:1A:2B:3C:4D:5G", mac_out);
    if (parse_rc != -1)
    {
        (void)printf("FAIL: \"00:1A:2B:3C:4D:5G\" parse_rc=%d (expected -1)\n", parse_rc);
        exit_code = 1;
    }
    else
    {
        (void)printf("PASS: \"00:1A:2B:3C:4D:5G\" -> parse_rc=%d\n", parse_rc);
    }

    parse_rc = parse_mac(NULL, mac_out);
    if (parse_rc != -1)
    {
        (void)printf("FAIL: NULL parse_rc=%d (expected -1)\n", parse_rc);
        exit_code = 1;
    }
    else
    {
        (void)printf("PASS: NULL -> parse_rc=%d\n", parse_rc);
    }

    return exit_code;
}
