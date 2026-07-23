#include "func_ptr_fsm.h"

#define TICK_NUM        (10U)

#define ARRAY_SIZE(a)   (uint32_t)(sizeof(a) / sizeof((a)[0]))

#define APP_SUCCESS     (0)

int32_t main(void)
{
    e_traffic_state_t light = RED;

    for (uint32_t tick = 1U; tick <= TICK_NUM; tick++)
    {
        run_traffic_fsm(tick, &light);
    }

    return APP_SUCCESS;
}