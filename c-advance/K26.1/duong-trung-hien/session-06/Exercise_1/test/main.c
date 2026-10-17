#include "timer_callback.h"

#define APP_SUCCESS     (0U)
#define LIMIT_TICK      (10U)
#define TICK_ALARM_1    (5U)
#define TICK_ALARM_2    (3U)

static void My_Alarm_Function()
{
    printf("[ALARM] Timer fired at tick %u!\n", Timer_GetCurrentTick());
}

int main()
{
    printf("--- Test 1: Alarm at tick %u, run for %u ticks ---\n", TICK_ALARM_1, LIMIT_TICK);
    Timer_Register(TICK_ALARM_1, My_Alarm_Function);

    for (uint32_t i = 1U; i <= LIMIT_TICK; i++)
    {
        Timer_Tick();
        if (i == 3U)
        {
            Timer_Register(TICK_ALARM_1, My_Alarm_Function);
        }
    }

    printf("\n--- Test 2: Reset, then new alarm at tick %u ---\n", TICK_ALARM_2);
    Timer_Reset();
    Timer_Register(TICK_ALARM_2, My_Alarm_Function);
    
    for (uint32_t i = 1U; i <= LIMIT_TICK; i++)
    {
        Timer_Tick();
    }

    return APP_SUCCESS;
}