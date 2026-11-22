/**
 * @file traffic_light.h
 * @brief Traffic Light Controller Function Pointer FSM Interface.
 */

#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <stdint.h>

/**
 * @brief Traffic light states enumeration.
 */
typedef enum {
    RED = 0,       /**< Red state: stop traffic.      */
    GREEN = 1,     /**< Green state: proceed.         */
    YELLOW = 2,    /**< Yellow state: caution.        */
    NUM_STATES     /**< Sentinel value (total count). */
} traffic_state_t;

/**
 * @brief Uniform function pointer signature for traffic light state handlers.
 */
typedef void (*traffic_handler_t)(uint32_t tick, traffic_state_t *p_next_state);

/**
 * @brief Run one tick of the refactored traffic light Function Pointer FSM.
 * @param[in]     tick    Current simulation tick number.
 * @param[in,out] p_state Pointer to current state; updated by state handler.
 */
void RunTrafficFSM(uint32_t tick, traffic_state_t *p_state);

/**
 * @brief Legacy switch-case traffic light runner provided for comparison.
 * @param[in,out] p_state Pointer to current state.
 * @param[in]     tick    Current simulation tick number.
 */
void run_traffic_light_bad(traffic_state_t *p_state, uint32_t tick);

#endif /* TRAFFIC_LIGHT_H */
