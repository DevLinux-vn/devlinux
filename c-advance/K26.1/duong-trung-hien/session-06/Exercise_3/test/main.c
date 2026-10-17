#include "command_dispatcher.h"

#define APP_SUCCESS (0U)

int main()
{
    const char *p_command_str[] = 
    {
        "LED_ON",
        "LED_OFF",
        "MOTOR_START",
        "MOTOR_STOP",
        "STATUS",
        "REBOOT"
    };

    for (uint32_t i = 0U; i < ARRAY_SIZE(p_command_str); i++)
    {
        Dispatch_Command(p_command_str[i]);
    }

    return APP_SUCCESS;
}