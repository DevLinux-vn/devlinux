#include "traffic_fsm.h"
#include <stdbool.h>
#include <stdio.h>

#define LIGHT_PERIOD (3U)

/**
 * @brief Current tick of the traffic light state machine.
 *
 * Stores the current system tick used by the traffic light FSM to determine
 * state transition timing. The value is incremented after each execution
 * cycle and is initialized to 1.
 *
 * Declared as `static` to restrict its visibility to this source file.
 */
static uint32_t s_state_tick = 1U;

/**
 * @brief Function pointer type for traffic light state handlers.
 *
 * Each state handler executes the logic of one traffic light state and
 * determines the next state of the finite state machine.
 *
 * @param[in]     tick          Current system tick.
 * @param[in,out] p_next_state  Pointer to the current state. The handler
 *                              updates it if a state transition occurs.
 */
typedef void (*traffic_handler_t)(uint32_t tick, traffic_state_t *p_next_state);

/**
 * @brief Validate the current traffic light state.
 *
 * Checks that the state pointer is valid and that the state value is within
 * the valid FSM state range.
 *
 * @param[in] p_state Pointer to the current traffic light state.
 *
 * @return Validation result.
 * @retval true  The state is valid.
 * @retval false The pointer is NULL or the state is out of range.
 */
static bool validate(const traffic_state_t *p_state);

/**
 * @brief Handle the RED traffic light state.
 *
 * Prints the RED state message and transitions to GREEN after the
 * required number of ticks.
 *
 * @param[in]     tick          Current system tick.
 * @param[in,out] p_next_state  Pointer to the current state.
 */
static void State_Red(uint32_t tick, traffic_state_t *p_next_state);

/**
 * @brief Handle the GREEN traffic light state.
 *
 * Prints the GREEN state message and transitions to YELLOW after the
 * required number of ticks.
 *
 * @param[in]     tick          Current system tick.
 * @param[in,out] p_next_state  Pointer to the current state.
 */
static void State_Green(uint32_t tick, traffic_state_t *p_next_state);

/**
 * @brief Handle the YELLOW traffic light state.
 *
 * Prints the YELLOW state message and transitions back to RED.
 *
 * @param[in]     tick          Current system tick.
 * @param[in,out] p_next_state  Pointer to the current state.
 */
static void State_Yellow(uint32_t tick, traffic_state_t *p_next_state);

/**
 * @brief Traffic light finite state machine dispatch table.
 *
 * Maps each traffic light state to its corresponding state handler.
 * The table is stored in read-only memory by declaring it as
 * static const.
 */
static const traffic_handler_t TrafficFSM[NUM_STATES] = {
    [RED]    = State_Red,
    [GREEN]  = State_Green,
    [YELLOW] = State_Yellow
};

static bool validate(const traffic_state_t *p_state)
{
    bool result = false;

    if (NULL == p_state)
    {
        printf("[ERROR] Pointer input is NULL!\n");
    }
    else if (*p_state >= NUM_STATES)
    {
        printf("[ERROR] Invalid state: %d!\n", *p_state);
    }
    else
    {
        result = true;
    }

    return result;
}

static void State_Red(uint32_t tick, traffic_state_t *p_next_state)
{
    if (validate(p_next_state))
    {
        printf("[RED]    Tick %u — Stop! Holding for 3 ticks.\n", tick);

        if ((s_state_tick % LIGHT_PERIOD) == 0)
        {
            *p_next_state = GREEN;
        }
    }
}

static void State_Green(uint32_t tick, traffic_state_t *p_next_state)
{
    if (validate(p_next_state))
    {
        printf("[GREEN]  Tick %u — Go!  Holding for 3 ticks.\n", tick);

        if ((s_state_tick % LIGHT_PERIOD) == 0)
        {
            *p_next_state = YELLOW;
        }
    }
}

static void State_Yellow(uint32_t tick, traffic_state_t *p_next_state)
{
    if (validate(p_next_state))
    {
        printf("[YELLOW] Tick %u — Slow down!\n", tick);
        *p_next_state = RED;
    }
}

void RunTrafficFSM(uint32_t tick, traffic_state_t *p_state)
{
    if (validate(p_state))
    {
        traffic_state_t rev_state = *p_state;
        TrafficFSM[*p_state](tick, p_state);

        if (rev_state != *p_state)
        {
            s_state_tick = 1U;
        }
        else
        {
            s_state_tick++;
        }
    }
}