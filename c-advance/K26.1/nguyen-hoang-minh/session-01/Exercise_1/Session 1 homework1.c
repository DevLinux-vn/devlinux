#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

/*
 * MISRA Directive 4.12: no malloc/free/realloc anywhere in this file —
 * every object below is a fixed-size, stack-allocated automatic variable.
 */

#define IPV4_DECIMAL_BASE      (10U)
#define IPV4_OCTET_MAX         (255U)
#define IPV4_OCTET_SHIFT_BITS  (8U)
#define IPV4_EXPECTED_ADDRESS  (0xC0A80132U)

/**
 * @brief Parse a single IPv4 octet starting at *p_index within ip_str.
 *
 * Reads 1 to 3 decimal digits, rejects leading zeros (e.g. "01"), validates
 * the accumulated value never exceeds 255, and reports how many characters
 * were consumed.
 *
 * @param[in]  ip_str     Null-terminated string being parsed.
 * @param[in,out] p_index Index to start reading from; advanced past the
 *                        digits consumed on success.
 * @param[out] p_octet    Parsed octet value (0-255) on success.
 * @return true on success, false if the octet is malformed or out of range.
 */
static bool parse_octet(const char *ip_str, size_t *p_index, uint8_t *p_octet)
{
    // declare all locals at the top of the function to satisfy MISRA Rule 8.7
    size_t   idx;    
    uint32_t value;
    uint8_t  digit_count;
    bool     result;

    /* MISRA Rule 9.1: every automatic-storage local is set before it is read. */
    idx = *p_index;
    value = 0U;
    digit_count = 0U;
    result = true;

    /*
     * ARR30-C / MISRA Directive 4.14: ip_str[idx + 1U] never reads past the
     * buffer because ip_str is guaranteed null-terminated by parse_ipv4's
     * caller contract — if ip_str[idx] is not '\0', ip_str[idx + 1U] is at
     * worst the terminating '\0' itself, never out of bounds.
     */
    if ((ip_str[idx] == '0') && (ip_str[idx + 1U] >= '0') && (ip_str[idx + 1U] <= '9'))
    {
        /* Leading zero on a multi-digit octet (e.g. "01") is rejected. */
        result = false;
    }
    else
    {
        /*
         * MISRA Rule 21.7 / CERT MSC24-C: no atoi()/atol() — digits are
         * accumulated one at a time so every partial value can be validated.
         */
        while ((ip_str[idx] >= '0') && (ip_str[idx] <= '9'))
        {
            if (digit_count >= 3U)
            {
                /* More than 3 digits cannot be a valid octet. */
                result = false;
                break;
            }

            /*
             * CERT INT32-C: check the accumulated value against
             * IPV4_OCTET_MAX immediately after each multiply-add, so an
             * out-of-range octet (e.g. "999") is rejected before it could
             * ever approach uint32_t overflow.
             */
            value = (value * IPV4_DECIMAL_BASE) + (uint32_t)(ip_str[idx] - '0');

            if (value > IPV4_OCTET_MAX)
            {
                /* MISRA Directive 4.14: reject octets outside 0-255. */
                result = false;
                break;
            }

            idx++;
            digit_count++;
        }

        if (digit_count == 0U)
        {
            /* No digits were consumed at all. */
            result = false;
        }
    }

    if (result)
    {
        /*
         * MISRA Rule 10.3 / CERT INT31-C: value is only narrowed to
         * uint8_t after the loop above has already proven
         * value <= IPV4_OCTET_MAX (255), so no data is lost or
         * misinterpreted by this conversion.
         */
        *p_octet = (uint8_t)value;
        *p_index = idx;
    }

    return result;
}

/**
 * @brief Parse an IPv4 address string into a 32-bit unsigned integer.
 *
 * Declared with external linkage on purpose: this is the public API of the
 * module (see the required prototype), intended to be called from other
 * translation units via a shared header.
 *
 * @param[in]  ip_str    Null-terminated ASCII string (e.g., "192.168.1.50").
 * @param[out] p_ip_out  Pointer to store the parsed 32-bit IP address.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out) // NOLINT(misc-use-internal-linkage)
{
    int8_t  status;
    size_t  index;
    uint8_t octet_count;
    uint8_t octet;
    uint32_t ip_value;

    if ((ip_str == NULL) || (p_ip_out == NULL))
    {
        /*
         * MISRA Directive 4.11 / CERT EXP34-C: both pointer arguments are
         * validated before either is ever dereferenced.
         *
         * This early return is the one deliberate exception to MISRA
         * Rule 15.5 (single point of exit): it is a pure guard clause with
         * no cleanup to perform, so returning immediately is clearer and
         * safer than threading a "not yet validated" state through the
         * rest of the function. Every other path below funnels through
         * the single `return status;` at the end.
         */
        return -1;
    }

    /* MISRA Rule 9.1: initialize all locals before they are read. */
    status = 0;
    index = 0U;
    octet_count = 0U;
    ip_value = 0U;

    while (octet_count < 4U)
    {
        if (!parse_octet(ip_str, &index, &octet))
        {
            /* MISRA Directive 4.14: malformed/out-of-range octet rejected. */
            status = -1;
            break;
        }

        ip_value = (ip_value << IPV4_OCTET_SHIFT_BITS) | (uint32_t)octet;
        octet_count++;

        if (octet_count < 4U)
        {
            if (ip_str[index] != '.')
            {
                /*
                 * MISRA Directive 4.14: the '.' delimiter is mandatory
                 * between octets — untrusted input that omits it (or has
                 * too few/too many octets) is rejected here.
                 */
                status = -1;
                break;
            }

            index++;
        }
    }

    if (status == 0)
    {
        if (ip_str[index] != '\0')
        {
            /*
             * MISRA Directive 4.14: trailing characters after the 4th
             * octet (e.g. "1.2.3.4.5" or "1.2.3.4x") make the whole
             * address invalid.
             */
            status = -1;
        }
    }

    if (status == 0)
    {
        *p_ip_out = ip_value;
    }

    /* MISRA Rule 15.5: single point of exit for the validated-input path. */
    return status;
}

/*
 * CERT STR31-C: not applicable here — parse_ipv4() never copies ip_str
 * into a local buffer, so there is no fixed-size destination that could
 * overflow. All parsing reads directly from the caller-owned,
 * null-terminated string.
 */

/**
 * @brief Simple test harness exercising the expected-output cases.
 *
 * @return 0 if all checks pass, non-zero if any check fails.
 */
int main(void)
{
    int8_t   parse_rc;
    uint32_t ip_out;
    int      exit_code;

    exit_code = 0;
    ip_out = 0U;

    /* CERT ERR33-C: parse_ipv4()'s return value is always checked below. */
    parse_rc = parse_ipv4("192.168.1.50", &ip_out);
    if ((parse_rc != 0) || (ip_out != IPV4_EXPECTED_ADDRESS))
    {
        (void)printf("FAIL: \"192.168.1.50\" parse_rc=%d ip_out=0x%08X\n", parse_rc, ip_out);
        exit_code = 1;
    }
    else
    {
        (void)printf("PASS: \"192.168.1.50\" -> 0x%08X\n", ip_out);
    }

    parse_rc = parse_ipv4("256.0.0.1", &ip_out);
    if (parse_rc != -1)
    {
        (void)printf("FAIL: \"256.0.0.1\" parse_rc=%d (expected -1)\n", parse_rc);
        exit_code = 1;
    }
    else
    {
        (void)printf("PASS: \"256.0.0.1\" -> parse_rc=%d\n", parse_rc);
    }

    parse_rc = parse_ipv4(NULL, &ip_out);
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
