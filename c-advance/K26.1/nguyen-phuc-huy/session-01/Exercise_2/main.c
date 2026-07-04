/**## Exercise_2 [build]
*
*### Problem Statement
*
**Safe Network Address Parser — MAC Address to `uint8_t[6]`**
*
**Scenario:**
*Same embedded networking scenario as Exercise 1. Here, we parse MAC addresses received as ASCII strings into a 6-byte array safely, without triggering undefined behavior.
*
**Requirements:**
*
*Write a C program that implements a MAC address parser with the following prototype:
*
*```c
*int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out);
*```
*
**Rules:**
*- Check for `NULL` pointers defensively.
*- Support both uppercase and lowercase hex digits (`0-9`, `a-f`, `A-F`).
*- Validate structure (ensure correct placement of `:` or `-` delimiters).
*- Ensure you do not overflow `p_mac_out` or perform pointer arithmetic past the end of the input string (`mac_str`).
*- Return `0` on success, or a negative error code on failure.
*- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix, Doxygen comments).
*- Use `cppcheck` and `clang-tidy` to analysis, and make sure there are no warning or error message.
*- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.
*### Expected Output
*
*```
*parse_mac("00:1A:2B:3C:4D:5E", mac_out)  -> returns  0, mac_out = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E}
*parse_mac("00-1a-2b-3c-4d-5e", mac_out)  -> returns  0, mac_out = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E}
*parse_mac("00:1A:2B:3C:4D",    mac_out)  -> returns -1  (Not enough octets)
*parse_mac("00:1A:2B:3C:4D:5E:6F", mac_out) -> returns -1  (Too many octets)
*parse_mac("00:1A:2B:3C:4D:5G", mac_out)  -> returns -1  (Invalid hex character 'G')
*parse_mac(NULL,                 mac_out)  -> returns -1  (Null input)
*```
*
*Exit code: `0` on success, non-zero on error.
*
*/







#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>





#define APP_SUCCESS                  (0)
#define APP_FAILURE                  (-1)

#define FUNC_SUCCESS                 (0)   
#define FUNC_FAILURE                 (-1)

#define OCTET_0_OFFSET                (2U)
#define OCTET_1_OFFSET                (5U)
#define OCTET_2_OFFSET                (8U)
#define OCTET_3_OFFSET                (11U)
#define OCTET_4_OFFSET                (14U)
#define MAX_DECIMAL_OCTET             (255U)

#define MAX_MAC_STRING_LENGHT         (18U)

#define ARRAY_SIZE(a)                 (sizeof(a)/(sizeof((a)[0])))
#define MAX_ARRAY_SIZE                (6U) 


#define IS_HEX_NUMBER(c)               (('0' <= (c)) && ((c) <= '9'))                           
#define IS_HEX_UPPERCASE(c)            (('A' <= (c)) && ((c) <= 'F'))
#define IS_HEX_LOWERCASE(c)            (('a' <= (c)) && ((c) <= 'f'))

#define NIBBLE_BITS                    (4U)
#define HEX_NUMBER_TO_DECIMAL_OFFSET   ('0')
#define HEX_UPPERCASE_TO_DECIMAL_OFFSET ('7')           
#define HEX_LOWERCASE_TO_DECIMAL_OFFSET ('W')



/**
 * @brief Parse a MAC address string into a 6-byte array.
 *
 * @param[in]  p_mac_str  Null-terminated ASCII string, for example
 *                        "00:1A:2B:3C:4D:5E" or "00-1a-2b-3c-4d-5e".
 * @param[out] p_mac_out  Pointer to a 6-byte array to store the parsed MAC address.
 * @param[in] size        Size of array out 
 * @return int8_t.
 *
 * @retval FUNC_SUCCESS Normal operation.
 * @retval FUNC_FAILURE Invalid input:
 *                        - p_mac_str is NULL.
 *                        - p_mac_out is NULL.
 *                        - Invalid hexadecimal character.
 *                        - Missing byte.
 *                        - Missing delimiter.
 *                        - Mixed or misplaced delimiter.
 *                        - Extra trailing characters.
 */
int8_t parse_mac(const char *p_mac_str, uint8_t *p_mac_out, uint16_t size);


/**
 * @brief This function is used to check string lenght of mac = 18 bytes inlcude '\0'
 * Check string_mac start != NULL and end with '\0'       
 * Check delimiter of mac in index 2, 5, 8, 11, 14
 * Index:                        0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  
 * Example:                      0  0  :  1  A  :  2  B  :  3   C   :   4   D   :   5   E  '\0'      (00:1A:2B:3C:4D:5E)
 * 
 * @param[in] p_mac_str   Pointer to string of Mac address           
 * 
 * @return bool
 * @retval true: MAC format is correct
 * @retval false: MAC format is incorrect
 */
static bool is_valid_string_mac(const char *p_mac_str);
/**
 * @brief Converse char of MAC to decimal value and check out of range of decimal in hex(0-9) and (a-f)or(A-F)
 * 
 * @param[in] mac_char hex character need process
 * @param[in] p_val Pointer to value will be returned after conversing 
 * @return int8_t
 * @retval FUNC_SUCCESS converse successfully
 * @retval FUNC_FAILURE out of range 
 */
static int8_t converse_char_to_dec(char mac_char, uint8_t *p_val);


/**
 * @brief Push decimal after conversing by converse char to dec in to the nibble
 *        If nibble is high, value will be shifted to left 4 bits
 *        If nibble is low, value will be ored with octet value
 * @param p_octet_address address of octet is needed process
 * @param val value of decimal when conversing 
 * @param is_high_nibble check nibble iss low or high
 */
static void push_dec_to_nibble(uint8_t *p_octet_address, uint8_t val, bool is_high_nibble);



/**
 * @brief Run built-in MAC parser test cases.
 *
 * This function parses a predefined list of MAC address strings and prints
 * either the parsed 6-byte value or an invalid-address message.
 *
 * @return APP_SUCCESS if all MAC strings are parsed successfully.
 * @return APP_FAILURE if any MAC string is invalid.
 */
int32_t main()
{
    const char *p_mac[] = {
            "00:1A:2B:3C:4D:5E",
            "00-1a-2b-3c-4d-5e",
            "00:1A:2B:3C:4D",
            "00:1A:2B:3C:4D:5E:6F",
            "00:1A:2B:3C:4D:5G"
        };
        uint8_t mac_out[MAX_ARRAY_SIZE] = {0U};
        int32_t exit_code = APP_SUCCESS;

        for (uint32_t i = 0U; i < ARRAY_SIZE(p_mac); i++)
        {
            int8_t ret = parse_mac(p_mac[i], mac_out, ARRAY_SIZE(mac_out));

            if (FUNC_SUCCESS != ret)
            {
                printf("Invalid MAC address: %s\n", p_mac[i]);
                exit_code = FUNC_FAILURE;
            }
            else
            {
                printf("MAC address  : %s\n", p_mac[i]);
                printf("Parsed value : ");
        
                for (uint8_t byte_idx = 0U; byte_idx < MAX_ARRAY_SIZE; byte_idx++)
                {
                    printf("0x%02X", (uint32_t)mac_out[byte_idx]);
        
                    if (byte_idx < (MAX_ARRAY_SIZE - 1U))
                    {
                        printf(" ");
                    }
                }
        
                printf("\n");
            }
        }
        return exit_code;
}


static bool is_valid_string_mac(const char *p_mac_str)
{
    int8_t ret = FUNC_SUCCESS;
    const char *p_start_address = NULL;
    if(NULL == p_mac_str )
    {
        ret = false;
    }
    else
    {
        uint8_t string_lenght = 0;
        p_start_address = p_mac_str;
        for(; '\0' != *p_mac_str; p_mac_str++)
        {
            string_lenght++;
        }
        string_lenght++;
        p_mac_str = p_start_address;
        if( MAX_MAC_STRING_LENGHT == string_lenght)
        {
            if(   ((':' == *(p_mac_str + OCTET_0_OFFSET)) 
                && (':' == *(p_mac_str + OCTET_1_OFFSET))
                && (':' == *(p_mac_str + OCTET_2_OFFSET))
                && (':' == *(p_mac_str + OCTET_3_OFFSET))
                && (':' == *(p_mac_str + OCTET_4_OFFSET)))
                || (('-' == *(p_mac_str + OCTET_0_OFFSET))
                && ('-' == *(p_mac_str + OCTET_1_OFFSET))
                && ('-' == *(p_mac_str + OCTET_2_OFFSET))
                && ('-' == *(p_mac_str + OCTET_3_OFFSET))
                && ('-' == *(p_mac_str + OCTET_4_OFFSET))))
            {
                ret = true;
            }
            else
            {
                ret = false;
            }    
        }
        else
        {
            ret = false;
        }  
    }
    return ret;
}

int8_t parse_mac(const char *p_mac_str, uint8_t *p_mac_out, uint16_t size)
{
    int8_t ret = FUNC_SUCCESS;
    bool is_octet_full = false;
    if( (NULL == p_mac_out) || 
        (NULL == p_mac_str) ||
        (MAX_ARRAY_SIZE > size))
    {
        ret = FUNC_FAILURE;
    }
    else if(is_valid_string_mac(p_mac_str))
    {
        while('\0' != *p_mac_str)
        {
            uint8_t temp_decimal = 0;
            ret = converse_char_to_dec(*p_mac_str, &temp_decimal);
            if(FUNC_FAILURE == ret)
            {   
                break;
            }
            push_dec_to_nibble(p_mac_out, temp_decimal, is_octet_full);
            is_octet_full = !is_octet_full;
            p_mac_str++;
            if((':' == *p_mac_str) || ('-' == *p_mac_str))
            {
                p_mac_out++;
                p_mac_str++;
            }
        }
    }

    return ret;
}



static int8_t converse_char_to_dec(char mac_char, uint8_t *p_val)
{
    int8_t ret = FUNC_FAILURE;
    if(IS_HEX_NUMBER(mac_char))
    {
        *p_val = mac_char - HEX_NUMBER_TO_DECIMAL_OFFSET;
        ret = FUNC_SUCCESS;
    }
    else if(IS_HEX_UPPERCASE(mac_char))
    {
        *p_val = mac_char - HEX_UPPERCASE_TO_DECIMAL_OFFSET;
        ret = FUNC_SUCCESS;
    }
    else if (IS_HEX_LOWERCASE(mac_char))
    {
        *p_val = mac_char - HEX_LOWERCASE_TO_DECIMAL_OFFSET;
        ret = FUNC_SUCCESS;
    }
    else
    {
        ret = FUNC_FAILURE;
    }
    return ret;
}



static void push_dec_to_nibble(uint8_t *p_octet_address, uint8_t val, bool is_high_nibble)
{
    if(true ==is_high_nibble)
    {
        *p_octet_address = *p_octet_address | val;
    }
    else
    {
        *p_octet_address = *p_octet_address | (val << NIBBLE_BITS);
    }
}
