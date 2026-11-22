/**
 * @file wifi_fsm.h
 * @brief Scalable WiFi Connection State Machine Interface.
 */

#ifndef WIFI_FSM_H
#define WIFI_FSM_H

#include <stdint.h>

/**
 * @brief WiFi state machine states enumeration.
 */
typedef enum {
    WIFI_INIT       = 0, /**< Initializing state.       */
    WIFI_CONNECTING = 1, /**< Connecting to AP state.   */
    WIFI_CONNECTED  = 2, /**< Connected link state.     */
    WIFI_ERROR      = 3, /**< Error / failure state.    */
    WIFI_MAX_STATES      /**< Sentinel (total states).  */
} WIFI_STATE;

/**
 * @brief Uniform function pointer signature for WiFi state handlers.
 */
typedef uint32_t (*wifi_handler_t)(uint8_t input, WIFI_STATE *p_next_state);

/**
 * @brief Run one step of the WiFi state machine.
 * @details Validates state bounds, guards function pointer, and dispatches to handler.
 * @param[in]     input   Hardware event input (1 = link up, 0 = link down/fail).
 * @param[in,out] p_state Pointer to the current state; updated upon transition.
 * @return 0 on success, 0xFF on invalid state.
 */
uint32_t RunStateMachine(uint8_t input, WIFI_STATE *p_state);

/**
 * @brief Convert WiFi state enum to human-readable string.
 * @param[in] state State enum value.
 * @return String literal name of the state.
 */
const char *WiFi_GetStateName(WIFI_STATE state);

#endif /* WIFI_FSM_H */
