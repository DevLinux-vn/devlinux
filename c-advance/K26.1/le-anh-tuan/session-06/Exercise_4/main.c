/**
 * @file main.c
 * @brief Function Pointer FSM traffic light controller.
 *
 * This program demonstrates the refactoring of a switch-case traffic
 * light controller into a function-pointer-based finite state machine.
 *
 * The original switch-case implementation is retained for comparison,
 * while the refactored FSM is used during normal execution.
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @enum traffic_state_t
 * @brief Defines all states of the traffic light controller.
 *
 * NUM_STATES is the sentinel value representing the total number
 * of valid states.
 */
typedef enum
{
    RED = 0,
    GREEN,
    YELLOW,
    NUM_STATES
} traffic_state_t;


/**
 * @typedef traffic_handler_t
 * @brief Function pointer type used by the traffic-light FSM.
 *
 * @param tick Current simulation tick.
 * @param p_next_state Pointer to the current/next state.
 */
typedef void (*traffic_handler_t)(
    uint32_t tick,
    traffic_state_t *p_next_state
);


/**
 * @brief Execute the RED state.
 *
 * @param tick Current simulation tick.
 * @param p_next_state Pointer to the traffic state.
 */
static void State_Red(
    uint32_t tick,
    traffic_state_t *p_next_state
);


/**
 * @brief Execute the GREEN state.
 *
 * @param tick Current simulation tick.
 * @param p_next_state Pointer to the traffic state.
 */
static void State_Green(
    uint32_t tick,
    traffic_state_t *p_next_state
);


/**
 * @brief Execute the YELLOW state.
 *
 * @param tick Current simulation tick.
 * @param p_next_state Pointer to the traffic state.
 */
static void State_Yellow(
    uint32_t tick,
    traffic_state_t *p_next_state
);


/**
 * @brief Original switch-case implementation.
 *
 * This function is retained as the original implementation so that
 * the refactored function-pointer FSM can be compared against it.
 *
 * @param p_state Pointer to the current traffic-light state.
 * @param tick Current simulation tick.
 */
static void run_traffic_light_bad(
    traffic_state_t *p_state,
    uint32_t tick
);


/**
 * @brief Run one tick of the traffic-light FSM.
 *
 * The function validates the state pointer, checks the state bounds,
 * validates the selected function pointer, and then dispatches the
 * appropriate state function.
 *
 * @param tick Current simulation tick.
 * @param p_state Pointer to the current traffic-light state.
 */
static void RunTrafficFSM(
    uint32_t tick,
    traffic_state_t *p_state
);


/**
 * @brief Function-pointer dispatch table for the traffic FSM.
 *
 * The table is static because it is private to this translation unit.
 * It is const so that it may be stored in read-only memory.
 */
static const traffic_handler_t TrafficFSM[NUM_STATES] =
{
    [RED] = State_Red,
    [GREEN] = State_Green,
    [YELLOW] = State_Yellow
};


/**
 * @brief Execute one tick using the original switch-case controller.
 *
 * The implementation is intentionally kept equivalent to the provided
 * starting code. A NULL pointer guard is included for safety.
 *
 * @param p_state Pointer to the current traffic-light state.
 * @param tick Current simulation tick.
 */
static void run_traffic_light_bad(
    traffic_state_t *p_state,
    uint32_t tick
)
{
    if (p_state == NULL)
    {
        return;
    }

    switch (*p_state)
    {
        case RED:
        {
            (void)printf(
                "[RED]    Tick %" PRIu32
                " — Stop! Holding for 3 ticks.\n",
                tick
            );

            if ((tick % 3U) == 0U)
            {
                *p_state = GREEN;
            }

            break;
        }

        case GREEN:
        {
            (void)printf(
                "[GREEN]  Tick %" PRIu32
                " — Go!  Holding for 3 ticks.\n",
                tick
            );

            if ((tick % 3U) == 0U)
            {
                *p_state = YELLOW;
            }

            break;
        }

        case YELLOW:
        {
            (void)printf(
                "[YELLOW] Tick %" PRIu32
                " — Slow down!\n",
                tick
            );

            *p_state = RED;

            break;
        }

        default:
        {
            break;
        }
    }
}


/**
 * @brief Handle the RED traffic-light state.
 *
 * The controller transitions from RED to GREEN whenever the
 * current tick is divisible by three.
 *
 * @param tick Current simulation tick.
 * @param p_next_state Pointer to the traffic state to update.
 */
static void State_Red(
    uint32_t tick,
    traffic_state_t *p_next_state
)
{
    (void)printf(
        "[RED]    Tick %" PRIu32
        " — Stop! Holding for 3 ticks.\n",
        tick
    );

    if ((tick % 3U) == 0U)
    {
        *p_next_state = GREEN;
    }
}


/**
 * @brief Handle the GREEN traffic-light state.
 *
 * The controller transitions from GREEN to YELLOW whenever the
 * current tick is divisible by three.
 *
 * @param tick Current simulation tick.
 * @param p_next_state Pointer to the traffic state to update.
 */
static void State_Green(
    uint32_t tick,
    traffic_state_t *p_next_state
)
{
    (void)printf(
        "[GREEN]  Tick %" PRIu32
        " — Go!  Holding for 3 ticks.\n",
        tick
    );

    if ((tick % 3U) == 0U)
    {
        *p_next_state = YELLOW;
    }
}


/**
 * @brief Handle the YELLOW traffic-light state.
 *
 * The YELLOW state lasts one tick and then transitions to RED.
 *
 * @param tick Current simulation tick.
 * @param p_next_state Pointer to the traffic state to update.
 */
static void State_Yellow(
    uint32_t tick,
    traffic_state_t *p_next_state
)
{
    (void)printf(
        "[YELLOW] Tick %" PRIu32
        " — Slow down!\n",
        tick
    );

    *p_next_state = RED;
}


/**
 * @brief Run one iteration of the function-pointer FSM.
 *
 * Safety checks are performed before the dispatch table is accessed:
 *
 * - p_state must not be NULL.
 * - The state must be less than NUM_STATES.
 * - The selected handler must not be NULL.
 *
 * @param tick Current simulation tick.
 * @param p_state Pointer to the current traffic-light state.
 */
static void RunTrafficFSM(
    uint32_t tick,
    traffic_state_t *p_state
)
{
    traffic_handler_t p_handler;

    if (p_state == NULL)
    {
        return;
    }

    if (*p_state >= NUM_STATES)
    {
        return;
    }

    p_handler = TrafficFSM[*p_state];

    if (p_handler == NULL)
    {
        return;
    }

    p_handler(tick, p_state);
}


/**
 * @brief Program entry point.
 *
 * Initializes the state to RED and executes the refactored
 * function-pointer FSM for ten ticks.
 *
 * The original switch-case function is intentionally referenced
 * but not called so that it remains available in this file for
 * comparison without producing duplicate output.
 *
 * @return 0 on successful completion.
 */
int main(void)
{
    traffic_state_t state = RED;
    uint32_t tick;

    /*
     * Keep the original implementation available for comparison
     * without executing it during the normal program run.
     */
    (void)&run_traffic_light_bad;

    for (tick = 1U; tick <= 10U; ++tick)
    {
        RunTrafficFSM(tick, &state);
    }

    return 0;
}