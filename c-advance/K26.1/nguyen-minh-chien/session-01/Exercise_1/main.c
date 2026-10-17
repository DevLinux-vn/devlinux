#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
/**
 * @brief Parse an IPv4 address string into a 32-bit unsigned integer.
 *
 * @param[in]  ip_str    Null-terminated ASCII string (e.g., "192.168.1.50").
 * @param[out] p_ip_out  Pointer to store the parsed 32-bit IP address.
 * @return 0 on success, -1 on invalid input.
 */
static int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out)
{
    if (ip_str == NULL || p_ip_out == NULL)
    {
        return -1;
    }

    uint8_t  octet_index = 0U;
    uint32_t octet_value = 0U;
    uint32_t ip_result   = 0U;
    bool     has_digit   = false;

    while (*ip_str != '\0')
    {
        if (*ip_str >= '0' && *ip_str <= '9')
        {
            if (octet_value > 25U)
            {
                return -1;
            }
            octet_value = octet_value * 10U + (uint32_t)(*ip_str - '0');
            if (octet_value > 255U)
            {
                return -1;
            }

            /*Đánh dấu octet này đã có ít nhất 1 chữ số */
            has_digit = true;

        }
        else if (*ip_str == '.')
        {
            /* Nếu chưa có chữ số nào trước dấu '.' → lỗi */
            /* Ví dụ: ".168.1.50" hoặc "192..1.50" */
            if (!has_digit)
            {
                return -1;
            }

            /* kiểm tra số lượng octet */
            if (octet_index >= 3U)
            {
                return -1;
            }

            /* Lưu octet hiện tại vào đúng vị trí bit */
            ip_result |= (octet_value << (24U - (octet_index * 8U)));

            /* Reset để đọc octet tiếp theo */
            octet_value = 0U;
            octet_index++;

            /* Reset has_digit cho octet tiếp theo */
            has_digit = false;
        }
        else
        {
            return -1; /* ký tự lạ */
        }

        ip_str++;
    }

    /* Sau vòng lặp: kiểm tra đủ 4 octet và octet cuối có chữ số */
    if (octet_index != 3U || !has_digit)
    {
        return -1;
    }

    /*  index 0 → shift 24, index 1 → shift 16, index 2 → shift 8, index 3 → shift ??? */
    ip_result |= (octet_value << 0U);

    *p_ip_out = ip_result;

    return 0;
}

int main(void)
{
    uint32_t ip_out = 0U;
    int8_t result;

    /* Test 1 */
    ip_out = 0U;
    result = parse_ipv4("192.168.1.50", &ip_out);
    printf("Test 1: result=%d, ip=0x%08X\n", result, ip_out);

    /* Test 2 */
    ip_out = 0U;
    result = parse_ipv4("256.0.0.1", &ip_out);
    printf("Test 2: result=%d, ip=0x%08X\n", result, ip_out);

    /* Test 3 */
    ip_out = 0U;
    result = parse_ipv4(NULL, &ip_out);
    printf("Test 3: result=%d, ip=0x%08X\n", result, ip_out);
    /* Test 4 */
    ip_out = 0U;
    result = parse_ipv4("0.0.0.0",&ip_out);
    printf("Test 4: result=%d, ip=0x%08X\n", result, ip_out);
    /* Test 5 */
    ip_out = 0U;
    result = parse_ipv4("192.168.0.1",&ip_out);
    printf("Test 5: result=%d, ip=0x%08X\n", result, ip_out);
    return 0;
}