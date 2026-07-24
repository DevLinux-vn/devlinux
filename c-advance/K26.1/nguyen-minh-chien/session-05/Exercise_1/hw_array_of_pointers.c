#include <stdio.h>
#include <stdint.h>
/**
 * @brief list error
 */
typedef enum
{
    ERR_OK = 0,      /**< Không có lỗi */
    ERR_TIMEOUT,     /**< Lỗi timeout */
    ERR_HW_FAIL,     /**< Lỗi phần cứng */
    ERR_COUNT        /**< Số lượng mã lỗi hợp lệ (không phải mã lỗi thật) */
} error_code_t;

/**
 * @brief Bảng ánh xạ từ error_code_t sang chuỗi mô tả.
 * @note  Thứ tự phần tử PHẢI khớp với error_code_t.
 */
static const char * const p_error_strings[] =
{
    [ERR_OK]      = "OK",
    [ERR_TIMEOUT] = "TIMEOUT_ERROR",
    [ERR_HW_FAIL] = "HARDWARE_FAILURE"
};

#define ERROR_STRING_COUNT (sizeof(p_error_strings) / sizeof(p_error_strings[0]))

static const char * const UNKNOWN_ERROR_STR = "UNKNOWN_ERROR";

/**
 * @brief  Chuyển đổi mã lỗi thành chuỗi mô tả tương ứng.
 * @param  err_code Mã lỗi cần tra cứu.
 * @return Con trỏ hằng tới chuỗi mô tả lỗi; trả về "UNKNOWN_ERROR"
 *         nếu err_code nằm ngoài phạm vi hợp lệ.
 */
const char *get_error_string(uint8_t err_code)
{
    const char *p_result = UNKNOWN_ERROR_STR;  

    if (err_code < ERROR_STRING_COUNT )
    {
        p_result = p_error_strings[err_code];
    }

    return p_result;
}

/**
 * @brief Điểm vào chương trình, minh họa tra cứu bảng lỗi.
 * @return 0 khi thành công.
 */
int main(void)
{
    uint8_t test_code_1 = ERR_TIMEOUT;
    uint8_t test_code_2 = 99U;

    printf("Error code %u: %s\n", (unsigned int)test_code_1, get_error_string(test_code_1));
    printf("Error code %u: %s\n", (unsigned int)test_code_2, get_error_string(test_code_2));

    return 0;
}