#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

static bool is_digit(char c)
{
    return ((c >= '0') && (c <= '9'));
}

int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out)
{
    uint32_t octets[4U] = {0U, 0U, 0U, 0U};
    uint32_t octet_value = 0U;
    uint32_t digit_count = 0U;
    uint32_t pos = 0U;
    uint32_t ret = 0U;

    if((ip_str == NULL) || (p_ip_out == NULL)){
        ret = -1;
    }
    else {
        for(uint32_t i = 0U; i < 4U; i++){
            octet_value = 0U;
            digit_count = 0U;
            while(is_digit(ip_str[pos])){
                uint32_t digit = (uint32_t)(ip_str[pos] - '0');
                octet_value = (octet_value * 10U) + digit;
                if(octet_value > 255U){
                    ret = -1;
                    break;
                }
                pos++;
                digit_count++;
            }
            if(ret != 0U){
                break;
            }
            if(digit_count == 0U){
                ret = -1;
                break;
            }
            octets[i] = octet_value;
            if(i < 3U){
                if(ip_str[pos] != '.'){
                    ret = -1;
                    break;
                }
                pos++;
            }
            else {
                if(ip_str[pos] != '\0'){
                    ret = -1;
                    break;
                }
            }
        }
        if(ret == 0){
            *p_ip_out = ((uint32_t)octets[0] << 24U) | ((uint32_t)octets[1] << 16U) | ((uint32_t)octets[2] << 8U) | ((uint32_t)octets[3]);
        }
    }
    return ret;
}


int main(void)
{
    uint32_t ip_out = 0U;
    int8_t ret = -1;

    ret = parse_ipv4("192.168.1.50", &ip_out);
    printf("ret = %d, ip_out = 0x%08X\n", ret, ip_out);

    ret = parse_ipv4("255.0.0.1", &ip_out);
    printf("ret = %d, ip_out = 0x%08X\n", ret, ip_out);

    ret = parse_ipv4("192.168.1", &ip_out);
    printf("ret = %d\n", ret);

    ret = parse_ipv4("192.168.1.1.5", &ip_out);
    printf("ret = %d\n", ret);

    ret = parse_ipv4("192..1.1", &ip_out);
    printf("ret = %d\n", ret);

    ret = parse_ipv4(NULL, &ip_out);
    printf("ret = %d\n", ret);

    return 0;
}
