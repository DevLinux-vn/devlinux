#ifndef SCALABLE_FSM_H
#define SCALABLE_FSM_H

#include <stdint.h>

/**
 * @brief Wi-Fi connection states used by the finite-state machine.
 */
typedef enum e_wifi_state
{
    WIFI_INIT       = 0U,   /**< Initial state used to initialize the Wi-Fi module. */
    WIFI_CONNECTING,        /**< Wi-Fi connection attempt is in progress. */
    WIFI_CONNECTED,         /**< Wi-Fi connection has been established. */
    WIFI_ERROR,             /**< Wi-Fi connection failed after reaching the retry limit. */
    WIFI_MAX_STATES         /**< Number of supported Wi-Fi states. */
} e_wifi_state_t;

/**
 * @brief Wi-Fi operation result codes.
 */
typedef enum e_errcode
{
    WIFI_ERRCODE_SUCCESS = 0U,  /**< Operation completed successfully. */
    WIFI_ERRCODE_WAITING,       /**< Operation is still in progress. */
    WIFI_ERRCODE_FAILURE        /**< Operation failed. */
} e_errcode_t;

/**
 * @brief Execute one step of the Wi-Fi finite-state machine.
 *
 * Validates the current state, invokes the corresponding state handler,
 * and updates the state when a transition condition is met.
 *
 * @param[in]     input   Wi-Fi event input: 1 indicates that the link is up;
 *                        0 indicates that the link is down or the connection
 *                        attempt failed.
 * @param[in,out] p_state Pointer to the current Wi-Fi state. The state may be
 *                        updated by the executed state handler.
 *
 * @return FSM execution result.
 * @retval WIFI_ERRCODE_SUCCESS The current state was processed successfully.
 * @retval WIFI_ERRCODE_WAITING The FSM is waiting for another connection attempt.
 * @retval WIFI_ERRCODE_FAILURE The state pointer, state value, or input is invalid.
 */
e_errcode_t run_state_machine(const uint8_t input, e_wifi_state_t *const p_state);

#endif