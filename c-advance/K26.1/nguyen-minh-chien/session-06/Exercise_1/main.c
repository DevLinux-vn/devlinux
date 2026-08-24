#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * @brief decla
 */
typedef void (*timer_callback_t)(void);

void Timer_Register(uint32_t expire_at_tick, timer_callback_t callback);
void Timer_Tick(void);
void Timer_Reset(void);
bool Timer_IsRunning(void);

static struct
{
    uint32_t         expire_at_tick;  /**< Tick count at which callback fires. */
    uint32_t         current_tick;    /**< Current elapsed tick count. */
    timer_callback_t on_expire;       /**< Callback to invoke on expiry. */
    bool             is_running;      /**< True if a timer is active. */
} s_timer = { 0U, 0U, NULL, false };

/**
 * @brief Register a callback to fire after a given number of ticks.
 * @param[in] expire_at_tick Tick number at which the callback fires.
 * @param[in] callback       Function pointer to invoke when the timer expires.
 * @return None.
 */
void Timer_Register(uint32_t expire_at_tick, timer_callback_t callback)
{
    if (s_timer.is_running == true)
    {
        printf("[WARN] Timer already running! Ignoring new registration.\n");
    }
    else
    {
        s_timer.expire_at_tick = expire_at_tick;
        s_timer.on_expire = callback;
        s_timer.is_running = true;
    }
}

/**
 * @brief Advance the timer by one tick. Invokes callback if expired.
 * @return None.
 */
void Timer_Tick(void)
{
    s_timer.current_tick++;

    if ((s_timer.is_running == true) && (s_timer.current_tick == s_timer.expire_at_tick))
    {
        if (s_timer.on_expire != NULL)
        {
            s_timer.on_expire();
        }
    }
}

/**
 * @brief Cancel any active timer and clear its state.
 * @return None.
 */
void Timer_Reset(void)
{
    s_timer.expire_at_tick = 0U;
    s_timer.current_tick   = 0U;
    s_timer.on_expire      = NULL;
    s_timer.is_running     = false;
}

/**
 * @brief Query whether a timer is currently active.
 * @return true if a timer is running, false otherwise.
 */
bool Timer_IsRunning(void)
{
    return s_timer.is_running;
}

/**
 * @brief Callback invoked when the first timer expires.
 * @return None.
 */
static void My_Alarm_Function(void)
{
    printf("[ALARM] Timer fired at tick 5!\n");
}

/**
 * @brief Callback invoked when the second timer expires.
 * @return None.
 */
static void My_Second_Alarm_Function(void)
{
    printf("[ALARM] Second alarm fired at tick 3!\n");
}

static void Dummy_Callback(void)
{
    printf("[ALARM] This should never print!\n");
}

int main(void)
{
    uint32_t i;

    printf("--- Test 1: Alarm at tick 5, run for 10 ticks ---\n");
    Timer_Register(5U, My_Alarm_Function);

    for (i = 1U; i <= 10U; i++)
    {
        printf("Tick %u...\n", i);
        Timer_Tick();

        if (i == 6U)
        {
            Timer_Register(99U, Dummy_Callback);
        }
    }

    printf("\n--- Test 2: Reset, then new alarm at tick 3 ---\n");
    Timer_Reset();
    printf("[TIMER] Reset.\n");

    Timer_Register(3U, My_Second_Alarm_Function);

    for (i = 1U; i <= 3U; i++)
    {
        printf("Tick %u...\n", i);
        Timer_Tick();
        
    }

    return 0;
}