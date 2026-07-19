#include <stdbool.h>
#include <stdio.h>

#include "scalable_fsm.h"

/**
 * @brief Input value indicating that the Wi-Fi connection succeeded.
 */
#define CONNECTED_SIGNAL    (1U)

/**
 * @brief Maximum number of failed Wi-Fi connection attempts.
 */
#define ATTEMPT_NUM         (3U)

/**
 * @brief Wi-Fi FSM state-handler function pointer type.
 *
 * Represents a state handler that processes an input signal and updates
 * the current Wi-Fi state when a transition condition is met.
 *
 * @param[in]     input   Input signal processed by the current state.
 * @param[in,out] p_state Pointer to the current Wi-Fi state.
 *
 * @return State-handler execution result.
 * @retval WIFI_ERRCODE_SUCCESS The state was processed successfully.
 * @retval WIFI_ERRCODE_WAITING The FSM is waiting for another connection attempt.
 * @retval WIFI_ERRCODE_FAILURE An invalid parameter, state, or input was detected.
 */
typedef e_errcode_t (*wifi_handler_t)(const uint8_t input, e_wifi_state_t *const p_state);

/**
 * @brief Validate a Wi-Fi state pointer and its current state.
 *
 * Checks that the state pointer is not NULL and that the current state
 * matches the state expected by the calling state handler.
 *
 * @param[in] p_state        Pointer to the current Wi-Fi state.
 * @param[in] expected_state State expected by the calling handler.
 * @param[in] p_func_name    Name of the calling function used in error messages.
 *
 * @return State validation result.
 * @retval WIFI_ERRCODE_SUCCESS The state pointer is valid and the current
 *                              state matches the expected state.
 * @retval WIFI_ERRCODE_FAILURE The state pointer is NULL or the current
 *                              state does not match the expected state.
 */
static e_errcode_t validate_state(const e_wifi_state_t *const p_state, const e_wifi_state_t expected_state, const char *const p_func_name);

/**
 * @brief Handle the Wi-Fi initialization state.
 *
 * Initializes the Wi-Fi state machine and transitions from
 * WIFI_INIT to WIFI_CONNECTING.
 *
 * @param[in]     input   Input signal. This parameter is unused.
 * @param[in,out] p_state Pointer to the current Wi-Fi state.
 *
 * @return State-handler execution result.
 * @retval WIFI_ERRCODE_SUCCESS The state transitioned to WIFI_CONNECTING.
 * @retval WIFI_ERRCODE_FAILURE The state pointer is NULL or the current
 *                              state is not WIFI_INIT.
 */
static e_errcode_t wifi_state_init(const uint8_t input, e_wifi_state_t *const p_state);

/**
 * @brief Handle the Wi-Fi connecting state.
 *
 * Transitions to WIFI_CONNECTED when the connection succeeds. When the
 * connection fails, the handler retries until the maximum number of
 * attempts is reached, then transitions to WIFI_ERROR.
 *
 * @param[in]     input   Connection result: 1 for success or 0 for failure.
 * @param[in,out] p_state Pointer to the current Wi-Fi state.
 *
 * @return State-handler execution result.
 * @retval WIFI_ERRCODE_SUCCESS The connection succeeded or the retry limit
 *                              was reached and a state transition occurred.
 * @retval WIFI_ERRCODE_WAITING The connection failed but another attempt
 *                              remains available.
 * @retval WIFI_ERRCODE_FAILURE The state pointer, current state, or input
 *                              value is invalid.
 */
static e_errcode_t wifi_state_connecting(const uint8_t input, e_wifi_state_t *const p_state);

/**
 * @brief Handle the Wi-Fi connected state.
 *
 * Keeps the FSM in WIFI_CONNECTED while the connection remains available.
 * Transitions to WIFI_CONNECTING when the connection is lost.
 *
 * @param[in]     input   Connection status: 1 for connected or 0 for disconnected.
 * @param[in,out] p_state Pointer to the current Wi-Fi state.
 *
 * @return State-handler execution result.
 * @retval WIFI_ERRCODE_SUCCESS The connected state was processed successfully.
 * @retval WIFI_ERRCODE_FAILURE The state pointer is NULL or the current
 *                              state is not WIFI_CONNECTED.
 */
static e_errcode_t wifi_state_connected(const uint8_t input, e_wifi_state_t *const p_state);

/**
 * @brief Handle the Wi-Fi error state.
 *
 * Performs error recovery and transitions from WIFI_ERROR to WIFI_INIT
 * so that the Wi-Fi connection sequence can restart.
 *
 * @param[in]     input   Input signal. This parameter is unused.
 * @param[in,out] p_state Pointer to the current Wi-Fi state.
 *
 * @return State-handler execution result.
 * @retval WIFI_ERRCODE_SUCCESS The state transitioned to WIFI_INIT.
 * @retval WIFI_ERRCODE_FAILURE The state pointer is NULL or the current
 *                              state is not WIFI_ERROR.
 */
static e_errcode_t wifi_state_error(const uint8_t input, e_wifi_state_t *const p_state);

/**
 * @brief Wi-Fi FSM state-handler table.
 *
 * Maps each supported Wi-Fi state to its corresponding state-handler
 * function. The constant table may be stored in read-only memory.
 */
static const wifi_handler_t wifi_fsm[] =
{
    [WIFI_INIT]       = &wifi_state_init,
    [WIFI_CONNECTING] = &wifi_state_connecting,
    [WIFI_CONNECTED]  = &wifi_state_connected,
    [WIFI_ERROR]      = &wifi_state_error
};

static e_errcode_t validate_state(const e_wifi_state_t *const p_state, const e_wifi_state_t expected_state, const char *const p_func_name)
{
    e_errcode_t result = WIFI_ERRCODE_SUCCESS;

    if (NULL == p_state)
    {
        printf("[ERROR] %s: p_state is NULL!\n", p_func_name);
        result = WIFI_ERRCODE_FAILURE;
    }
    else if (*p_state != expected_state)
    {
        printf("[ERROR] %s: Unexpected state: %d (Expected %d)!\n", p_func_name, *p_state, expected_state);
        result = WIFI_ERRCODE_FAILURE;
    }
    else
    {
        ; /**< Do nothing */
    }

    return result;
}

static e_errcode_t wifi_state_init(const uint8_t input, e_wifi_state_t *const p_state)
{
    e_errcode_t result = WIFI_ERRCODE_SUCCESS;

    /* Unused */
    (void)input;

    result = validate_state(p_state, WIFI_INIT, __func__);

    if (WIFI_ERRCODE_SUCCESS == result)
    {
        printf("[INIT] Initializing... -> CONNECTING\n");
        *p_state = WIFI_CONNECTING;
    }

    return result;
}

static e_errcode_t wifi_state_connecting(const uint8_t input, e_wifi_state_t *const p_state)
{
    e_errcode_t result = WIFI_ERRCODE_SUCCESS;
    
    result = validate_state(p_state, WIFI_CONNECTING, __func__);

    if (WIFI_ERRCODE_SUCCESS == result)
    {
        static uint8_t s_attempts = 0U;

        if ((0U != input) && (CONNECTED_SIGNAL != input))
        {
            printf("[ERROR] %s: Incorrect input. Should be 0 or 1!\n", __func__);
            result = WIFI_ERRCODE_FAILURE;
        }
        else if (CONNECTED_SIGNAL == input)
        {
            printf("[CONNECTING] Connected! Retry count reset.\n");
            
            /* Wi-Fi connected. */
            *p_state  = WIFI_CONNECTED;
            s_attempts = 0U;
        }
        else    /**< Input = 0 */
        {
            /* Retry until the maximum number of attempts is reached. */
            s_attempts++;

            if (s_attempts >= ATTEMPT_NUM)
            {
                printf("[CONNECTING] Attempt %u failed. -> ERROR\n", s_attempts);
                *p_state = WIFI_ERROR;

                /* Reset the counter after giving up. */
                s_attempts = 0U;
            }
            else
            {
                printf("[CONNECTING] Attempt %u failed. Retrying...\n", s_attempts);
                result = WIFI_ERRCODE_WAITING;
            }
        }
    }

    return result;
}

static e_errcode_t wifi_state_connected(const uint8_t input, e_wifi_state_t *const p_state)
{
    e_errcode_t result = WIFI_ERRCODE_SUCCESS;

    result = validate_state(p_state, WIFI_CONNECTED, __func__);

    if (WIFI_ERRCODE_SUCCESS == result)
    {
        if (0U == input)
        {
            printf("[CONNECTED] Link dropped. Reconnecting...\n");
            *p_state = WIFI_CONNECTING;
        }
    }

    return result;
}

static e_errcode_t wifi_state_error(const uint8_t input, e_wifi_state_t *const p_state)
{
    e_errcode_t result = WIFI_ERRCODE_SUCCESS;

    /* Unused */
    (void)input;

    result = validate_state(p_state, WIFI_ERROR, __func__);

    if (WIFI_ERRCODE_SUCCESS == result)
    {
        printf("[ERROR] Recovery. Restarting -> INIT\n");
        *p_state = WIFI_INIT;
    }

    return result;
}

/**
 * @brief Execute the handler associated with the current Wi-Fi state.
 */
e_errcode_t run_state_machine(const uint8_t input, e_wifi_state_t *const p_state)
{
    e_errcode_t result = WIFI_ERRCODE_SUCCESS;

    if (NULL == p_state)
    {
        printf("[ERROR] %s: p_state is NULL!\n", __func__);
        result = WIFI_ERRCODE_FAILURE;
    }
    else if ((uint8_t)WIFI_MAX_STATES <= (uint8_t)*p_state)
    {
        printf("[ERROR] %s: state = %u is invalid!\n", __func__, (uint8_t)*p_state);
        result = WIFI_ERRCODE_FAILURE;
    }
    else
    {
        result = wifi_fsm[*p_state](input, p_state);
    }

    return result;
}