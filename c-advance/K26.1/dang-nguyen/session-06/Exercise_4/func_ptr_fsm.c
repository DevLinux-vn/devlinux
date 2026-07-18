#include <stdio.h>
#include <stdbool.h>
#include "func_ptr_fsm.h"

/**
 * @brief Number of ticks for which the red and green lights remain active.
 */
#define LIGHT_PERIOD    (3U)

/**
 * @brief Traffic-light state handler function pointer type.
 *
 * Represents a state handler that processes the current tick and updates
 * the next traffic-light state when a transition condition is met.
 *
 * @param[in]     tick         Current FSM tick count.
 * @param[in,out] p_next_state Pointer to the current and next FSM state.
 */
typedef void (*traffic_handler_t)(const uint32_t tick, e_traffic_state_t *const p_next_state);

/**
 * @brief Validate a null-terminated string.
 *
 * @param[in] p_str Pointer to the string to validate.
 *
 * @return String validation result.
 * @retval true  The string pointer is not NULL and the string is not empty.
 * @retval false The string pointer is NULL or the string is empty.
 */
static bool validate_string(const char *const p_str);

/**
 * @brief Validate a traffic-light state pointer and its state value.
 *
 * Prints an error message when the state pointer is NULL or the state value
 * is outside the supported traffic-light states.
 *
 * @param[in] p_state     Pointer to the traffic-light state to validate.
 * @param[in] p_func_name Name of the calling function used in error messages.
 * @param[in] p_obj       Name of the validated object used in error messages.
 *
 * @return State validation result.
 * @retval true  The state pointer and state value are valid.
 * @retval false An input string, state pointer, or state value is invalid.
 */
static bool validate_state(const e_traffic_state_t *const p_state, const char *const p_func_name, const char *const p_obj);

/**
 * @brief Handle the red traffic-light state.
 *
 * Prints the red-light status and transitions to the green state when the
 * configured light period has elapsed.
 *
 * @param[in]     tick         Current FSM tick count.
 * @param[in,out] p_next_state Pointer to the current and next FSM state.
 */
static void state_red(const uint32_t tick, e_traffic_state_t *const p_next_state);

/**
 * @brief Handle the green traffic-light state.
 *
 * Prints the green-light status and transitions to the yellow state when
 * the configured light period has elapsed.
 *
 * @param[in]     tick         Current FSM tick count.
 * @param[in,out] p_next_state Pointer to the current and next FSM state.
 */
static void state_green(const uint32_t tick, e_traffic_state_t *const p_next_state);

/**
 * @brief Handle the yellow traffic-light state.
 *
 * Prints the yellow-light status and transitions immediately to the red
 * state.
 *
 * @param[in]     tick         Current FSM tick count.
 * @param[in,out] p_next_state Pointer to the current and next FSM state.
 */
static void state_yellow(const uint32_t tick, e_traffic_state_t *const p_next_state);

/**
 * @brief Traffic-light FSM state-handler table.
 *
 * Maps each traffic-light state to its corresponding state handler function.
 * The table is constant and may be stored in read-only memory.
 */
static const traffic_handler_t traffic_fsm[NUM_STATES] =
{
    [RED]    = &state_red,
    [GREEN]  = &state_green,
    [YELLOW] = &state_yellow
};

static bool validate_string(const char *const p_str)
{
    return ((NULL != p_str) && ('\0' != *p_str));
}

static bool validate_state(const e_traffic_state_t *const p_state, const char *const p_func_name, const char *const p_obj)
{
    bool result = false;

    if ((true == validate_string(p_func_name)) && (true == validate_string(p_obj)))
    {
        if (NULL == p_state)
        {
            printf("[ERROR] %s: %s is NULL!\n", p_func_name, p_obj);
        }
        else if ((RED != *p_state) && (GREEN != *p_state) && (YELLOW != *p_state))
        {
            printf("[ERROR] %s: Invalid state: %d!\n", p_func_name, *p_state);
        }
        else
        {
            result = true;
        }
    }

    return result;
}

static void state_red(const uint32_t tick, e_traffic_state_t *const p_next_state)
{
    if (true == validate_state(p_next_state, __func__, "p_next_state"))
    {
        printf("[RED]    Tick %u — Stop! Holding for 3 ticks.\n", tick);

        if ((tick % LIGHT_PERIOD) == 0U)
        {
            *p_next_state = GREEN;
        }
    }
}

static void state_green(const uint32_t tick, e_traffic_state_t *const p_next_state)
{
    if (true == validate_state(p_next_state, __func__, "p_next_state"))
    {
        printf("[GREEN]  Tick %u — Go! Holding for 3 ticks.\n", tick);

        if ((tick % LIGHT_PERIOD) == 0U)
        {
            *p_next_state = YELLOW;
        }
    }
}

static void state_yellow(const uint32_t tick, e_traffic_state_t *const p_next_state)
{
    if (true == validate_state(p_next_state, __func__, "p_next_state"))
    {
        printf("[YELLOW] Tick %u — Slow down!\n", tick);
        *p_next_state = RED;
    }
}

/**
 * @brief Execute the handler associated with the current traffic-light state.
 */
void run_traffic_fsm(const uint32_t tick, e_traffic_state_t *const p_state)
{
    if (true == validate_state(p_state, __func__, "p_state"))
    {
        traffic_fsm[*p_state](tick, p_state);
    }
}