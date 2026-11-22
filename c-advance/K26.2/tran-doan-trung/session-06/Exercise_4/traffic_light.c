/**
 * @file traffic_light.c
 * @brief Implementation of Traffic Light Function Pointer FSM.
 */

#include "traffic_light.h"
#include <stdio.h>

// cppcheck-suppress unusedFunction
void run_traffic_light_bad(traffic_state_t *p_state, uint32_t tick)
{
    if (NULL == p_state) {
        return;
    }

    switch (*p_state) {
        case RED:
            printf("[RED]    Tick %u — Stop! Holding for 3 ticks.\n", tick);
            if (tick % 3U == 0U) {
                *p_state = GREEN;
            }
            break;
        case GREEN:
            printf("[GREEN]  Tick %u — Go!  Holding for 3 ticks.\n", tick);
            if (tick % 3U == 0U) {
                *p_state = YELLOW;
            }
            break;
        case YELLOW:
            printf("[YELLOW] Tick %u — Slow down!\n", tick);
            *p_state = RED;
            break;
        default:
            *p_state = RED;
            break;
    }
}

/**
 * @brief Handler for RED state.
 * @param[in]     tick         Current tick number.
 * @param[in,out] p_next_state Pointer to next state.
 */
static void State_Red(uint32_t tick, traffic_state_t *p_next_state)
{
    printf("[RED]    Tick %u — Stop! Holding for 3 ticks.\n", tick);
    if (tick % 3U == 0U) {
        *p_next_state = GREEN;
    }
}

/**
 * @brief Handler for GREEN state.
 * @param[in]     tick         Current tick number.
 * @param[in,out] p_next_state Pointer to next state.
 */
static void State_Green(uint32_t tick, traffic_state_t *p_next_state)
{
    printf("[GREEN]  Tick %u — Go!  Holding for 3 ticks.\n", tick);
    if (tick % 3U == 0U) {
        *p_next_state = YELLOW;
    }
}

/**
 * @brief Handler for YELLOW state.
 * @param[in]     tick         Current tick number.
 * @param[in,out] p_next_state Pointer to next state.
 */
static void State_Yellow(uint32_t tick, traffic_state_t *p_next_state)
{
    printf("[YELLOW] Tick %u — Slow down!\n", tick);
    *p_next_state = RED;
}

/**
 * @brief Function pointer table mapped by state enum, stored in Flash (.rodata).
 */
static const traffic_handler_t TrafficFSM[NUM_STATES] = {
    [RED]    = State_Red,
    [GREEN]  = State_Green,
    [YELLOW] = State_Yellow
};

void RunTrafficFSM(uint32_t tick, traffic_state_t *p_state)
{
    /* MISRA Directive 4.11 / CERT EXP34-C: Guard pointer parameter against NULL */
    if (NULL == p_state) {
        return;
    }

    /* CERT ARR30-C: Bounds check before indexing table */
    if (*p_state >= NUM_STATES) {
        *p_state = RED; /* Error recovery */
        return;
    }

    traffic_handler_t p_handler = TrafficFSM[*p_state];
    /* MISRA Directive 4.11 / CERT EXP34-C: Guard function pointer against NULL */
    if (p_handler != NULL) {
        p_handler(tick, p_state);
    }
}
