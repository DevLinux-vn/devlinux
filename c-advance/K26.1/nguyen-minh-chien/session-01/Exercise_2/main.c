#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/**
 * @brief Parse a MAC address string into a 6-byte array.
 *
 * @param[in]  mac_str    Null-terminated ASCII string.
 * @param[out] p_mac_out  Pointer to a 6-byte array.
 * @return 0 on success, -1 on invalid input.
 */
static int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out);
int main(void)
{
    /* Định nghĩa một test case */
    typedef struct
    {
        const char *input;       /* chuỗi MAC đầu vào    */
        const char *description; /* mô tả test case      */
    } test_case_t;

    /* Danh sách các test case */
    const test_case_t tests[] = {
        { "00:1A:2B:3C:4D:5E",    "OK  - uppercase, dau :"          },
        { "00-1a-2b-3c-4d-5e",    "OK  - lowercase, dau -"          },
        { "00:1A:2B:3C:4D",       "ERR - thieu octet"               },
        { "00:1A:2B:3C:4D:5E:6F", "ERR - thua octet"                },
        { "00:1A:2B:3C:4D:5G",    "ERR - ky tu khong hop le"        },
        { NULL,                    "ERR - NULL pointer"              },
    };

    uint8_t num_tests = (uint8_t)(sizeof(tests) / sizeof(tests[0]));

    /* Chạy từng test case */
    for (uint8_t i = 0U; i < num_tests; i++)
    {
        uint8_t mac_out[6] = {0U, 0U, 0U, 0U, 0U, 0U};
        int8_t  result     = parse_mac(tests[i].input, mac_out);

        printf("[%s]\n", tests[i].description);

        if (result == 0)
        {
            printf("  -> returns 0, mac_out = {0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X}\n\n",
                   mac_out[0], mac_out[1], mac_out[2],
                   mac_out[3], mac_out[4], mac_out[5]);
        }
        else
        {
            printf("  -> returns %d\n\n", result);
        }
    }

    return 0;
}
static int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out)
{
    /* Bước 1: kiểm tra NULL */
    if (mac_str == NULL || p_mac_out == NULL)
    {
        return -1;
    }

    /* Bước 2: đếm độ dài */
    uint8_t len = 0U;
    const char *p_tmp = mac_str;
    while (*p_tmp != '\0')
    {
        p_tmp++;
        len++;
    }
    if (len != 17U)
    {
        return -1;
    }

    /* Bước 3: lấy delimiter */
    char delim = mac_str[2];
    if ((delim != ':') && (delim != '-'))
    {
        return -1;
    }

    /* Bước 4: vòng lặp 6 octet */
    for (uint8_t i = 0U; i < 6U; i++)
    {
        uint8_t pos = i * 3U;

        /* đọc 2 hex digit */
        char c_high = mac_str[pos];
        char c_low  = mac_str[pos + 1U];

        /* chuyển hex digit sang số */
        uint8_t high = 0U;
        uint8_t low  = 0U;

        if      (c_high >= '0' && c_high <= '9') { high = (uint8_t)(c_high - '0'); }
        else if (c_high >= 'A' && c_high <= 'F') { high = (uint8_t)(c_high - 'A' + 10); }
        else if (c_high >= 'a' && c_high <= 'f') { high = (uint8_t)(c_high - 'a' + 10); }
        else { return -1; }

        if      (c_low >= '0' && c_low <= '9') { low = (uint8_t)(c_low - '0'); }
        else if (c_low >= 'A' && c_low <= 'F') { low = (uint8_t)(c_low - 'A' + 10); }
        else if (c_low >= 'a' && c_low <= 'f') { low = (uint8_t)(c_low - 'a' + 10); }
        else { return -1; }

        p_mac_out[i] = (uint8_t)((high * 16U) + low);

        /* kiểm tra delimiter */
        if (i < 5U)
        {
            if (mac_str[pos + 2U] != delim)
            {
                return -1;
            }
        }
    }

    return 0;
}
