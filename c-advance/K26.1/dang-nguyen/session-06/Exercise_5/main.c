#include <stdio.h>
#include "scalable_fsm.h"

#define ARRAY_SIZE(a)   (uint32_t)(sizeof(a) / sizeof((a)[0]))

#define APP_SUCCESS     (0)
#define APP_FAILURE     (-1)

#define WIFI_INIT_STR        ("WIFI_INIT")
#define WIFI_CONNECTING_STR  ("WIFI_CONNECTING")
#define WIFI_CONNECTED_STR   ("WIFI_CONNECTED")
#define WIFI_ERROR_STR       ("WIFI_ERROR")

static const char *state_str[WIFI_MAX_STATES] =
{
    [WIFI_INIT]       = WIFI_INIT_STR,
    [WIFI_CONNECTING] = WIFI_CONNECTING_STR,
    [WIFI_CONNECTED]  = WIFI_CONNECTED_STR,
    [WIFI_ERROR]      = WIFI_ERROR_STR
};

int32_t main(void)
{
    uint8_t input_data[] = { 0, 0, 1, 0, 0, 0, 0, 1 };
    e_wifi_state_t state = WIFI_INIT;
    e_errcode_t api_ret = WIFI_ERRCODE_SUCCESS;
    int32_t ret = APP_SUCCESS;

    for (uint32_t step = 0U; step < ARRAY_SIZE(input_data); step++)
    {
        if ((WIFI_ERRCODE_SUCCESS == api_ret) || (WIFI_ERRCODE_WAITING == api_ret))
        {
            printf("[Step %u] %s State: %s\t\t\t| input = %u\n",
                    step,
                    (WIFI_ERROR == state) ? "(auto)" : "",
                    state_str[state],
                    input_data[step]);
            api_ret = run_state_machine(input_data[step], &state);
            printf("\n");
        }
    }

    if (WIFI_ERRCODE_FAILURE == api_ret)
    {
        ret = APP_FAILURE;
    }

    return ret;
}