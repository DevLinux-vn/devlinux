/**
 * @file main.c
 * @brief Application executing Traffic Light Controller Function Pointer FSM.
 */

#include "traffic_light.h"

/**
 * @brief Main entry point running 10-tick simulation.
 * @return 0 on success.
 */
int main(void)
{
    traffic_state_t state = RED;

    /* Simulate 10 ticks using the refactored Function Pointer FSM */
    for (uint32_t tick = 1U; tick <= 10U; ++tick) {
        RunTrafficFSM(tick, &state);
    }

    return 0;
}
