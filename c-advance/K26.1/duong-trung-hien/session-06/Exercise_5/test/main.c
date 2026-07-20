#include "wifi_manage.h"
#include <stdio.h>
#include <stdint.h>

#define APP_SUCCESS         (0U)
#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof(arr[0]))

int main()
{
    const uint8_t input_test[] = { 0U, 0U, 1U, 0U, 0U, 0U, 0U, 1U };
    wifi_state_t state = WIFI_INIT;


    for (uint32_t i = 0U; i < ARRAY_SIZE(input_test); i++)
    {
        RunStateMachine(input_test[i], &state);
    }

    return APP_SUCCESS;
}