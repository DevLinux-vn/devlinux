#ifndef WIFI_MANAGE_H
#define WIFI_MANAGE_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define WIFI_OK             (0U)
#define WIFI_RECONNECT      (1U)
#define WIFI_TIMEOUT        (2U)
#define WIFI_INVALID_PARAM  (3U)

/**
 * @brief WiFi connection state definitions.
 */
typedef enum {
    WIFI_INIT = 0,        /**< Initialize the WiFi subsystem.                    */
    WIFI_CONNECTING,      /**< Attempt to establish a WiFi connection.            */
    WIFI_CONNECTED,       /**< WiFi connection established and operational.       */
    WIFI_ERROR,           /**< Connection failed; waiting for recovery.           */
    WIFI_MAX_STATES       /**< Total number of valid WiFi states (sentinel).      */
} wifi_state_t;

/**
 * @brief Execute one iteration of the WiFi finite state machine.
 *
 * Dispatches the current state handler based on the current state, validates
 * the state index and function pointer, and updates the state if a transition
 * occurs.
 *
 * @param[in]     input    WiFi event input
 *                         (1 = connection available, 0 = connection unavailable).
 * @param[in,out] p_state  Pointer to the current WiFi state. The value is updated
 *                         if the state machine performs a state transition.
 *
 * @return
 * - 0U   : State machine executed successfully.
 * - 0xFF : Invalid state or dispatch error.
 */
uint32_t RunStateMachine(uint8_t input, wifi_state_t *p_state);

#endif /* WIFI_MANAGE_H */