/**
 * @file main.c
 * @brief Application demonstrating Software Timer Callback Registration.
 */

#include "timer.h"
#include <stdio.h>

/**
 * @brief First alarm callback function fired at tick 5.
 */
static void My_Alarm_Function(void)
{
    printf("[ALARM] Timer fired at tick 5!\n");
}

/**
 * @brief Second alarm callback function fired at tick 3 after reset.
 */
static void Second_Alarm_Function(void)
{
    printf("[ALARM] Second alarm fired at tick 3!\n");
}

/**
 * @brief Main function demonstrating timer callback registration and execution.
 * @return 0 on success.
 */
int main(void)
{
    printf("--- Test 1: Alarm at tick 5, run for 10 ticks ---\n");
    Timer_Register(5U, My_Alarm_Function);

    for (uint32_t i = 1U; i <= 10U; ++i) {
        Timer_Tick();
        if (6U == i) {
            /* Attempt second registration while timer is active */
            Timer_Register(8U, Second_Alarm_Function);
        }
    }

    printf("\n--- Test 2: Reset, then new alarm at tick 3 ---\n");
    bool is_active = Timer_IsRunning();
    (void)is_active;
    Timer_Reset();
    Timer_Register(3U, Second_Alarm_Function);

    for (uint32_t i = 1U; i <= 3U; ++i) {
        Timer_Tick();
    }

    return 0;
}
