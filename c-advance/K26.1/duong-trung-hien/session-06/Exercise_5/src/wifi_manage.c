#include "wifi_manage.h"

/**
 * @brief Function pointer type for a WiFi state handler.
 *
 * Each state handler processes the current input, determines the next state,
 * and returns a status code indicating the execution result.
 *
 * @param[in]  input         WiFi event input
 *                           (1 = connection available, 0 = connection unavailable).
 * @param[out] p_next_state  Pointer to the next WiFi state.
 *
 * @return
 * - 0U   : State executed successfully.
 * - 0xFF : State execution failed.
 */
typedef uint32_t (*wifi_handler_t)(uint8_t input, wifi_state_t *p_next_state);

/**
 * @brief Validate the current WiFi state.
 *
 * Checks whether the state pointer is valid and the state value is within the
 * valid range.
 *
 * @param[in] p_state Pointer to the current WiFi state.
 *
 * @retval true  The state is valid.
 * @retval false The state pointer is NULL or the state is invalid.
 */
static bool validate(const wifi_state_t *p_state);

/**
 * @brief Handle the WIFI_INIT state.
 *
 * Initializes the WiFi state machine, resets the retry counter, and transitions
 * to the WIFI_CONNECTING state.
 *
 * @param[in]  input         WiFi event input.
 * @param[out] p_next_state  Pointer to the next WiFi state.
 *
 * @return
 * - 0U   : State executed successfully.
 * - 0xFF : State execution failed.
 */
static uint32_t wifi_state_init(uint8_t input, wifi_state_t *p_next_state);

/**
 * @brief Handle the WIFI_CONNECTING state.
 *
 * Attempts to establish a WiFi connection. On success, transitions to
 * WIFI_CONNECTED. On repeated failures, transitions to WIFI_ERROR.
 *
 * @param[in]  input         WiFi event input.
 * @param[out] p_next_state  Pointer to the next WiFi state.
 *
 * @return
 * - 0U   : State executed successfully.
 * - 0xFF : State execution failed.
 */
static uint32_t wifi_state_connecting(uint8_t input, wifi_state_t *p_next_state);

/**
 * @brief Handle the WIFI_CONNECTED state.
 *
 * Monitors the WiFi connection. Remains connected while the link is active or
 * transitions back to WIFI_CONNECTING if the connection is lost.
 *
 * @param[in]  input         WiFi event input.
 * @param[out] p_next_state  Pointer to the next WiFi state.
 *
 * @return
 * - 0U   : State executed successfully.
 * - 0xFF : State execution failed.
 */
static uint32_t wifi_state_connected(uint8_t input, wifi_state_t *p_next_state);

/**
 * @brief Handle the WIFI_ERROR state.
 *
 * Performs error recovery and transitions the state machine back to
 * WIFI_INIT.
 *
 * @param[in]  input         WiFi event input.
 * @param[out] p_next_state  Pointer to the next WiFi state.
 *
 * @return
 * - 0U   : State executed successfully.
 * - 0xFF : State execution failed.
 */
static uint32_t wifi_state_error(uint8_t input, wifi_state_t *p_next_state);

/**
 * @brief WiFi state machine dispatch table.
 *
 * Maps each WiFi state to its corresponding handler function. The table is
 * declared as `static const` so it is stored in read-only memory.
 */
static const wifi_handler_t WifiFSM[WIFI_MAX_STATES] = {
    [WIFI_INIT]       = wifi_state_init,
    [WIFI_CONNECTING] = wifi_state_connecting,
    [WIFI_CONNECTED]  = wifi_state_connected,
    [WIFI_ERROR]      = wifi_state_error
};

static bool validate(const wifi_state_t *p_state)
{
    bool result = false;
    
    if (NULL == p_state)
    {
        printf("[ERROR] Pointer input is NULL!\n");
    }
    else if (*p_state >= WIFI_MAX_STATES)
    {
        printf("[ERROR] Invalid state: %d!\n", *p_state);
    }
    else
    {
        result = true;
    }

    return result;
}

static uint32_t wifi_state_init(uint8_t input, wifi_state_t *p_next_state)
{
    (void)input;
    if (validate(p_next_state))
    {
        *p_next_state = WIFI_CONNECTING;
        printf("[INIT] Initializing... -> CONNECTING\n");
    }
    else
    {
        return WIFI_INVALID_PARAM;
    }

    return WIFI_OK;
}

static uint32_t wifi_state_connecting(uint8_t input, wifi_state_t *p_next_state)
{
    if (validate(p_next_state))
    {
        static uint8_t retry_count = 0U;
        if (input == 1U)
        {
            retry_count = 0U;
            *p_next_state = WIFI_CONNECTED;
            printf("[CONNECTING] Connected! Retry count reset.\n");
        }
        else
        {
            retry_count++;
            if (retry_count >= 3U)
            {
                printf("[CONNECTING] Attempt %u failed. -> ERROR\n", retry_count);
                retry_count = 0U;
                *p_next_state = WIFI_ERROR;
                return WIFI_TIMEOUT;
            }

            printf("[CONNECTING] Attempt %u failed. Retrying...\n", retry_count);
            return WIFI_RECONNECT;
        }
    }
    else
    {
        return WIFI_INVALID_PARAM;
    }

    return WIFI_OK;
}

static uint32_t wifi_state_connected(uint8_t input, wifi_state_t *p_next_state)
{
    if (validate(p_next_state))
    {
        if (input == 1U)
        {
            printf("[CONNECTED] Online...\n");
        }
        else
        {
            *p_next_state = WIFI_CONNECTING;
            printf("[CONNECTED] Link dropped. Reconnecting...\n");
            return WIFI_RECONNECT;
        }
    }
    else
    {
        return WIFI_INVALID_PARAM;
    }

    return WIFI_OK;
}

static uint32_t wifi_state_error(uint8_t input, wifi_state_t *p_next_state)
{
    (void)input;
    if (validate(p_next_state))
    {
        *p_next_state = WIFI_INIT;
        printf("[ERROR] Recovery. Restarting -> INIT\n");
    }
    else
    {
        return WIFI_INVALID_PARAM;
    }

    return WIFI_OK;
}

uint32_t RunStateMachine(uint8_t input, wifi_state_t *p_state)
{
    if (validate(p_state))
    {
        WifiFSM[*p_state](input, p_state);
    }
    else
    {
        return WIFI_INVALID_PARAM;
    }

    return WIFI_OK;
}