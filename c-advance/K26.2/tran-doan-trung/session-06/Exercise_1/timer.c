/**
 * @file timer.c
 * @brief Implementation of Software Timer with Callback Registration Pattern.
 */

#include "timer.h"
#include <stdio.h>

/**
 * @brief Internal static timer state structure (Singleton pattern).
 * @details Invisible to other translation units, fully encapsulated.
 */
static struct {
    uint32_t         expire_at_tick; /**< Tick count at which the callback fires. */
    uint32_t         current_tick;   /**< Current elapsed tick count.             */
    timer_callback_t on_expire;      /**< Callback to invoke on expiry.           */
    bool             is_running;     /**< True if a timer is currently active.    */
} s_timer = { 0U, 0U, NULL, false };

bool Timer_IsRunning(void)
{
    return s_timer.is_running;
}

void Timer_Register(uint32_t expire_at_tick, timer_callback_t p_callback)
{
    if (Timer_IsRunning()) {
        printf("[WARN] Timer already running! Ignoring new registration.\n");
        return;
    }

    if (NULL == p_callback) {
        return;
    }

    s_timer.expire_at_tick = expire_at_tick;
    s_timer.current_tick   = 0U;
    s_timer.on_expire      = p_callback;
    s_timer.is_running     = true;
}

void Timer_Tick(void)
{
    if (!s_timer.is_running) {
        return;
    }

    s_timer.current_tick++;
    printf("Tick %u...\n", s_timer.current_tick);

    if (s_timer.current_tick == s_timer.expire_at_tick) {
        /* MISRA Directive 4.11 / CERT EXP34-C: Guard callback pointer against NULL */
        if (s_timer.on_expire != NULL) {
            timer_callback_t p_func = s_timer.on_expire;
            s_timer.on_expire = NULL; /* Prevent re-invocation */
            p_func();
        }
    }
}

void Timer_Reset(void)
{
    s_timer.expire_at_tick = 0U;
    s_timer.current_tick   = 0U;
    s_timer.on_expire      = NULL;
    s_timer.is_running     = false;
    printf("[TIMER] Reset.\n");
}
