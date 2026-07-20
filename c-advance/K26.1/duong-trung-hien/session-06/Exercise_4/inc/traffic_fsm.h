#ifndef TRAFFIC_FSM_H
#define TRAFFIC_FSM_H

#include <stdint.h>

/**
 * @brief Traffic light FSM states.
 */
typedef enum {
    RED = 0U,      /**< Red light state. */
    GREEN,         /**< Green light state. */
    YELLOW,        /**< Yellow light state. */
    NUM_STATES     /**< Total number of FSM states (sentinel). */
} traffic_state_t;

/**
 * @brief Execute one tick of the traffic light finite state machine.
 *
 * Dispatches the current state handler through the function pointer table.
 * The current state may be updated by the executed state handler.
 *
 * @param[in]     tick      Current system tick.
 * @param[in,out] p_state   Pointer to the current traffic light state.
 */
void RunTrafficFSM(uint32_t tick, traffic_state_t *p_state);



#endif /* TRAFFIC_FSM_H */