#include <stdint.h>
#include <stdio.h>
#include "dispatch.h"

#define APP_SUCCESS     (0)
#define APP_FAILURE     (1)

#define ARRAY_SIZE(a)   (uint32_t)(sizeof(a) / sizeof((a)[0]))

int32_t main(void)
{
    int32_t ret = APP_SUCCESS;
    int8_t cmd[] = { MAIN_MENU, SETTINGS_MENU, ABOUT_MENU, CMD_COUNT };

    for (uint32_t idx = 0U; idx < ARRAY_SIZE(cmd); idx++)
    {
        ret = ((dispatch_ui(cmd[idx]) == ERR_OK) ? APP_SUCCESS : APP_FAILURE);

        if (APP_SUCCESS != ret)
        {
            break;
        }
    }

    return ret;
}