#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Function pointer type for a timer callback.
 *
 * The callback takes no parameters and returns no value.
 */
typedef void (*timer_callback_t)(void);

/**
 * @brief Internal state of the software timer.
 *
 * The structure stores the expiry tick, current tick, registered
 * callback, and running state.
 *
 * The object has static storage duration and internal linkage so
 * it cannot be accessed directly from another translation unit.
 */
static struct
{
    uint32_t expire_at_tick;      /**< Tick at which the timer expires. */
    uint32_t current_tick;        /**< Current elapsed timer tick. */
    timer_callback_t on_expire;   /**< Callback invoked on timer expiry. */
    bool is_running;              /**< Indicates whether timer is active. */
} s_timer = {0U, 0U, NULL, false};

/**
 * @brief Register a callback to execute at a specified tick.
 *
 * If a timer is already running, the new registration is rejected
 * and the current timer remains unchanged.
 *
 * A NULL callback is also rejected.
 *
 * @param[in] expire_at_tick Tick at which the callback shall execute.
 * @param[in] p_callback     Pointer to the callback function.
 */
static void Timer_Register(
    uint32_t expire_at_tick,
    timer_callback_t p_callback)
{
    if (s_timer.is_running)
    {
        (void)printf(
            "[WARN] Timer already running! Ignoring new registration.\n");
    }
    else if (p_callback == NULL)
    {
        (void)printf(
            "[WARN] NULL callback! Ignoring registration.\n");
    }
    else
    {
        s_timer.expire_at_tick = expire_at_tick;
        s_timer.current_tick = 0U;
        s_timer.on_expire = p_callback;
        s_timer.is_running = true;
    }
}

/**
 * @brief Advance the software timer by one tick.
 *
 * If a timer is running, the current tick is incremented.
 * When the current tick equals the configured expiry tick,
 * the timer is stopped and the registered callback is invoked.
 *
 * The callback pointer is checked against NULL before invocation.
 */
static void Timer_Tick(void)
{
    timer_callback_t p_callback = NULL;

    if (s_timer.is_running)
    {
        ++s_timer.current_tick;

        if (s_timer.current_tick == s_timer.expire_at_tick)
        {
            /*
             * Save the callback locally before clearing the
             * timer state.
             */
            p_callback = s_timer.on_expire;

            s_timer.is_running = false;
            s_timer.on_expire = NULL;

            if (p_callback != NULL)
            {
                p_callback();
            }
        }
    }
}

/**
 * @brief Cancel the active timer and reset all timer state.
 */
static void Timer_Reset(void)
{
    s_timer.expire_at_tick = 0U;
    s_timer.current_tick = 0U;
    s_timer.on_expire = NULL;
    s_timer.is_running = false;
}

/**
 * @brief Determine whether the timer is currently running.
 *
 * @return true if the timer is active.
 * @return false if the timer is inactive.
 */
static bool Timer_IsRunning(void)
{
    return s_timer.is_running;
}

/**
 * @brief Callback function for the first alarm.
 */
static void My_Alarm_Function(void)
{
    (void)printf("[ALARM] Timer fired at tick 5!\n");
}

/**
 * @brief Callback function for the second alarm.
 */
static void My_Second_Alarm_Function(void)
{
    (void)printf("[ALARM] Second alarm fired at tick 3!\n");
}

/**
 * @brief Program entry point.
 *
 * Demonstrates:
 * - Timer registration.
 * - Timer state querying.
 * - Rejection of registration while a timer is active.
 * - Callback execution at the correct tick.
 * - Timer reset.
 * - Registration and execution of a second timer.
 *
 * @return 0 on successful completion.
 */
int main(void)
{
    uint32_t tick = 0U;

    /*
     * Test 1:
     * Register the first alarm for tick 5.
     */
    (void)printf(
        "--- Test 1: Alarm at tick 5, run for 10 ticks ---\n");

    Timer_Register(5U, My_Alarm_Function);

    if (Timer_IsRunning())
    {
        (void)printf("[TIMER] Timer is running.\n");
    }

    for (tick = 1U; tick <= 10U; ++tick)
    {
        (void)printf(
            "Tick %lu...\n",
            (unsigned long)tick);

        /*
         * Try registering another timer while the first
         * timer is still active.
         */
        if (tick == 3U)
        {
            Timer_Register(
                3U,
                My_Second_Alarm_Function);
        }

        Timer_Tick();
    }

    /*
     * Test 2:
     * Reset the timer and register another alarm.
     */
    (void)printf(
        "\n--- Test 2: Reset, then new alarm at tick 3 ---\n");

    Timer_Reset();
    (void)printf("[TIMER] Reset.\n");

    Timer_Register(
        3U,
        My_Second_Alarm_Function);

    for (tick = 1U; tick <= 5U; ++tick)
    {
        (void)printf(
            "Tick %lu...\n",
            (unsigned long)tick);

        Timer_Tick();
    }

    return 0;
}