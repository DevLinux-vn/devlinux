/**
 * @file main.c
 * @brief Application demonstrating Scalable WiFi Connection State Machine.
 */

#include "wifi_fsm.h"
#include <stdio.h>

/**
 * @brief Main entry point executing WiFi FSM test sequence.
 * @return 0 on success.
 */
int main(void)
{
    const uint8_t input_sequence[] = { 0U, 0U, 0U, 1U, 0U, 0U, 0U, 0U, 0U };
    const uint32_t total_steps = (uint32_t)(sizeof(input_sequence) / sizeof(input_sequence[0]));

    WIFI_STATE current_state = WIFI_INIT;

    for (uint32_t step = 0U; step < total_steps; ++step) {
        const uint8_t input = input_sequence[step];

        if (step > 0U) {
            printf("\n");
        }

        if (8U == step) {
            printf("[Step %u] (auto) State: %-10s | input=%u\n", step, WiFi_GetStateName(current_state), input);
        } else {
            printf("[Step %u] State: %-15s | input=%u\n", step, WiFi_GetStateName(current_state), input);
        }

        (void)RunStateMachine(input, &current_state);
    }

    return 0;
}
