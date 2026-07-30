#include "parse_mac.h"
#include <stddef.h>

int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out)
{
    if ((mac_str == NULL) || (p_mac_out == NULL))
    {
        return -1;
    }

    char separator = 0;

    for (int part = 0; part < 6; part++)
    {
        char ch1 = *mac_str++;
        char ch2 = *mac_str++;

        int high;
        int low;

        /* Đổi ký tự đầu sang số */
        if ((ch1 >= '0') && (ch1 <= '9'))
        {
            high = ch1 - '0';
        }
        else if ((ch1 >= 'A') && (ch1 <= 'F'))
        {
            high = ch1 - 'A' + 10;
        }
        else if ((ch1 >= 'a') && (ch1 <= 'f'))
        {
            high = ch1 - 'a' + 10;
        }
        else
        {
            return -1;
        }

        /* Đổi ký tự thứ hai sang số */
        if ((ch2 >= '0') && (ch2 <= '9'))
        {
            low = ch2 - '0';
        }
        else if ((ch2 >= 'A') && (ch2 <= 'F'))
        {
            low = ch2 - 'A' + 10;
        }
        else if ((ch2 >= 'a') && (ch2 <= 'f'))
        {
            low = ch2 - 'a' + 10;
        }
        else
        {
            return -1;
        }

        p_mac_out[part] = (uint8_t)((high * 16) + low);

        if (part < 5)
        {
            char ch = *mac_str++;

            if (separator == 0)
            {
                if ((ch != ':') && (ch != '-'))
                {
                    return -1;
                }

                separator = ch;
            }
            else
            {
                if (ch != separator)
                {
                    return -1;
                }
            }
        }
    }

    /* Không được còn ký tự sau MAC */
    if (*mac_str != '\0')
    {
        return -1;
    }

    return 0;
}