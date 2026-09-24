#include <stdio.h>
#include <stdint.h>

int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out)
{
    uint8_t digit_count = 0;
    uint8_t dot_count = 0;
    uint32_t value = 0;
    uint32_t ip = 0;
    
    if((ip_str == NULL) || (p_ip_out == NULL))
    {
        return -1;
    }
    while(*ip_str != '\0')
    {
        if((*ip_str >= '0') && (*ip_str <= '9'))
        {
            value = value * 10 + (*ip_str-'0');
            if(value > 255)
            {
                return -1;
            }
            digit_count++;
        }
        else if (*ip_str == '.')
        {
            if(digit_count == 0)
            {
                return -1;
            }
            ip = (ip << 8) | value;
            value = 0;
            digit_count = 0;
            dot_count++;
        }
        else
        {
            return -1;
        }
        ip_str++;
    }
    
    if (digit_count == 0)
    {
        return -1;
    }

    if (dot_count != 3)
    {
        return -1;
    }

    ip = (ip << 8) | value;

    *p_ip_out = ip;

    return 0;
}

int main()
{
    const char addr[] = "192.168.1.1";
    uint32_t ip_out = 0;
    parse_ipv4(addr, &ip_out);
    printf("IP hex = 0x%08X\n", ip_out);

    return 0;
}