#include "traffic_fsm.h"
#include <stdio.h>

#define APP_SUCCESS (0U)
#define TICK_LIMIT  (10U)

int main()
{
    traffic_state_t state = RED;

    for (uint32_t i = 1U; i <= TICK_LIMIT; i++)
    {
        RunTrafficFSM(i, &state);
    }
    return APP_SUCCESS;
}