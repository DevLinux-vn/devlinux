#ifndef FUNC_PTR_FSM_H
#define FUNC_PTR_FSM_H

#include <stdint.h>

typedef enum e_traffic_state
{
    RED        = 0U,
    GREEN,
    YELLOW,
    NUM_STATES
} e_traffic_state_t;

/**
 * @brief Run one tick of the traffic light FSM.
 * @param[in]     tick      Current tick number.
 * @param[in,out] p_state   Pointer to current state; updated by the state function.
 */
void run_traffic_fsm(const uint32_t tick, e_traffic_state_t *const p_state);

#endif