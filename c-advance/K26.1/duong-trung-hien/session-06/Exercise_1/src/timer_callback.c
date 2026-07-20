#include "timer_callback.h"

/**
 * @brief Internal software timer state.
 *
 * This static structure stores the current state of the software timer.
 * It is private to this source file and cannot be accessed directly by
 * other modules.
 */
static struct {
    uint32_t            expire_at_tick;
    uint32_t            current_tick;
    timer_callback_t    on_expire;
    bool                is_running;   
} s_timer = {0U, 0U, NULL, false};

void Timer_Register(uint32_t expire_at_tick, timer_callback_t callback)
{
    if (callback == NULL)
    {
        return;
    }
    
    if (Timer_IsRunning() == true)
    {
        printf("[WARN] Timer already running! Ignoring new registration.\n");
        return;
    }
    
    s_timer.expire_at_tick =    expire_at_tick;
    s_timer.on_expire =         callback;   
    s_timer.is_running =        true;
    s_timer.current_tick =      0U;
}

void Timer_Tick(void)
{
    if (Timer_IsRunning() != true)
    {
        return;
    }

    s_timer.current_tick++;
    printf("Tick %u...\n", s_timer.current_tick);

    if ((s_timer.current_tick) == (s_timer.expire_at_tick))
    {
        if (s_timer.on_expire != NULL)
        {
            s_timer.on_expire();
            s_timer.is_running = false;
            return;
        }
    }
}

void Timer_Reset(void)
{
    if (Timer_IsRunning() == true)
    {
        s_timer.expire_at_tick =    0U;
        s_timer.current_tick =      0U;
        s_timer.is_running =        false;
        printf("[TIMER] Reset.\n");
    }
}

bool Timer_IsRunning(void)
{
    return s_timer.is_running;
}

uint32_t Timer_GetCurrentTick(void)
{
    return s_timer.current_tick;
}