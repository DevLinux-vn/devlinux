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
 * @brief Run one step of the WiFi FSM.
 * @param[in]     input     Hardware/event input (1 = link up, 0 = link down/fail).
 * @param[in,out] p_state   Pointer to the current state; updated on transition.
 * @return 0 on success, 0xFF on invalid state.
 */
e_errcode_t run_state_machine(const uint8_t input, e_wifi_state_t *const p_state);

#endif