#include <stddef.h>
#include "parse_ipv4.h"

int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out)
{
    if ( (ip_str) == NULL || (p_ip_out) == NULL)
    {
        return -1;
    }

    uint32_t a = 0, b = 0, c = 0, d = 0;
    int part = 0;

    for(int i = 0; ip_str[i] != '\0'; i++)
    {
        char ch = ip_str[i];
        /*(c) - '0'; Mã ASCII thì lấy mã ASCII của số cần trừ mốc '0' thì ra được số cần gọi là quy đổi từ kí tự sang số */
        /*biến từng kí tự thành số*/
        if( (ch) >= '0' && (ch) <= '9') /*Nếu kí tự là chữ số*/
        {
            if ( (part) == 0 ) a = a * 10 + ( (ch) - '0');
            else if ( (part) == 1) b = b * 10 + ( (ch) - '0');
            else if ( (part) == 2) c = c * 10 + ( (ch) - '0');
            else d = d * 10 + ( (ch) - '0');
        } 
        else if ( (ch) == '.') /*gặp dấu chấm là bỏ qua*/
        {
            part++;
            if (part > 3) return -1;
        }
        else
        {
            return -1;
        }
    }

    if ( (a) > 255 || (b) > 255 || (c) > 255 || (d) > 255)
    {
        return -1;
    }

    *p_ip_out = (a << 24) | (b << 16) | (c << 8) | d;

    return 0;
}

