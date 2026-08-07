/**
 * @file main.c
 * @brief WiFi connection finite state machine using a function-pointer table.
 *
 * This program demonstrates a scalable WiFi connection FSM using:
 * - A function-pointer dispatch table.
 * - State passed explicitly through a pointer.
 * - A static local retry counter.
 * - State bounds checking.
 * - Function-pointer NULL checking.
 *
 * The implementation follows the FSM rules described in Exercise 5.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Maximum number of failed WiFi connection attempts.
 */
#define WIFI_MAX_RETRIES (3U)

/**
 * @brief Successful FSM execution status.
 */
#define WIFI_STATUS_OK (0U)

/**
 * @brief FSM execution error status.
 */
#define WIFI_STATUS_ERROR (0xFFU)

/**
 * @brief WiFi connection states.
 *
 * WIFI_MAX_STATES is a sentinel used for bounds checking and must remain
 * the final entry.
 */
typedef enum
{
    WIFI_INIT = 0,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_ERROR,
    WIFI_MAX_STATES
} WIFI_STATE;

/**
 * @brief Uniform WiFi state-handler function pointer type.
 *
 * @param[in] input Hardware/event input:
 *                  1 = WiFi link available,
 *                  0 = WiFi link unavailable.
 * @param[out] p_next_state Pointer receiving the next FSM state.
 *
 * @return WIFI_STATUS_OK on success.
 */
typedef uint32_t (*wifi_handler_t)(uint8_t input,
                                   WIFI_STATE *p_next_state);

/**
 * @brief Handle the WIFI_INIT state.
 *
 * The retry sequence starts fresh when the FSM begins from INIT.
 * The initial value of the static retry counter is zero. Furthermore,
 * any transition through WIFI_ERROR resets the retry counter before
 * returning to WIFI_INIT.
 *
 * @param[in] input Input value. Ignored in this state.
 * @param[out] p_next_state Pointer receiving WIFI_CONNECTING.
 *
 * @return WIFI_STATUS_OK.
 */
static uint32_t wifi_state_init(uint8_t input,
                                WIFI_STATE *p_next_state);

/**
 * @brief Handle the WIFI_CONNECTING state.
 *
 * A successful connection resets the retry counter and transitions to
 * WIFI_CONNECTED. A failed connection increments the retry counter.
 * Three consecutive failures transition the FSM to WIFI_ERROR.
 *
 * @param[in] input 1 for connection success, 0 for connection failure.
 * @param[out] p_next_state Pointer receiving the next FSM state.
 *
 * @return WIFI_STATUS_OK.
 */
static uint32_t wifi_state_connecting(uint8_t input,
                                      WIFI_STATE *p_next_state);

/**
 * @brief Handle the WIFI_CONNECTED state.
 *
 * When input remains 1, the FSM remains connected. When input becomes
 * 0, the link is considered lost and the FSM transitions back to
 * WIFI_CONNECTING.
 *
 * @param[in] input Current WiFi link status.
 * @param[out] p_next_state Pointer receiving the next FSM state.
 *
 * @return WIFI_STATUS_OK.
 */
static uint32_t wifi_state_connected(uint8_t input,
                                     WIFI_STATE *p_next_state);

/**
 * @brief Handle the WIFI_ERROR state.
 *
 * The error state automatically performs recovery by transitioning to
 * WIFI_INIT on the next FSM tick.
 *
 * @param[in] input Input value. Ignored in this state.
 * @param[out] p_next_state Pointer receiving WIFI_INIT.
 *
 * @return WIFI_STATUS_OK.
 */
static uint32_t wifi_state_error(uint8_t input,
                                 WIFI_STATE *p_next_state);

/**
 * @brief Convert a WiFi FSM state to a printable string.
 *
 * @param[in] state WiFi FSM state.
 *
 * @return Pointer to a constant state-name string.
 */
static const char *wifi_state_name(WIFI_STATE state);

/**
 * @brief Execute one step of the WiFi finite state machine.
 *
 * The function checks the state value before indexing the dispatch
 * table and verifies that the selected function pointer is not NULL
 * before calling it.
 *
 * Invalid states are recovered by returning the FSM to WIFI_INIT.
 *
 * @param[in] input Current WiFi hardware/event input.
 * @param[in,out] p_state Pointer to the current FSM state.
 *
 * @return WIFI_STATUS_OK on success or WIFI_STATUS_ERROR on failure.
 */
static uint32_t RunStateMachine(uint8_t input,
                                WIFI_STATE *p_state);

/**
 * @brief WiFi FSM dispatch table.
 *
 * The table is static const so the implementation does not require
 * writable RAM for the dispatch entries.
 */
static const wifi_handler_t WifiFSM[WIFI_MAX_STATES] =
{
    [WIFI_INIT]       = wifi_state_init,
    [WIFI_CONNECTING] = wifi_state_connecting,
    [WIFI_CONNECTED]  = wifi_state_connected,
    [WIFI_ERROR]      = wifi_state_error
};

/**
 * @brief Handle the WIFI_INIT state.
 *
 * @param[in] input Input value. Ignored.
 * @param[out] p_next_state Pointer receiving WIFI_CONNECTING.
 *
 * @return WIFI_STATUS_OK.
 */
static uint32_t wifi_state_init(uint8_t input,
                                WIFI_STATE *p_next_state)
{
    (void)input;

    printf("[INIT] Initializing... -> CONNECTING\n");

    *p_next_state = WIFI_CONNECTING;

    return WIFI_STATUS_OK;
}

/**
 * @brief Handle connection attempts and retry counting.
 *
 * @param[in] input 1 for success or 0 for failure.
 * @param[out] p_next_state Pointer receiving the next state.
 *
 * @return WIFI_STATUS_OK.
 */
static uint32_t wifi_state_connecting(uint8_t input,
                                      WIFI_STATE *p_next_state)
{
    static uint32_t s_retry_count = 0U;

    if (input == 1U)
    {
        s_retry_count = 0U;
        *p_next_state = WIFI_CONNECTED;

        printf("[CONNECTING] Connected! Retry count reset.\n");
    }
    else
    {
        s_retry_count++;

        if (s_retry_count >= WIFI_MAX_RETRIES)
        {
            printf(
                "[CONNECTING] Attempt %u failed. -> ERROR\n",
                (unsigned int)s_retry_count);

            s_retry_count = 0U;
            *p_next_state = WIFI_ERROR;
        }
        else
        {
            printf(
                "[CONNECTING] Attempt %u failed. Retrying...\n",
                (unsigned int)s_retry_count);

            *p_next_state = WIFI_CONNECTING;
        }
    }

    return WIFI_STATUS_OK;
}

/**
 * @brief Handle an established WiFi connection.
 *
 * @param[in] input Current WiFi link status.
 * @param[out] p_next_state Pointer receiving the next state.
 *
 * @return WIFI_STATUS_OK.
 */
static uint32_t wifi_state_connected(uint8_t input,
                                     WIFI_STATE *p_next_state)
{
    if (input == 1U)
    {
        printf("[CONNECTED] WiFi online.\n");

        *p_next_state = WIFI_CONNECTED;
    }
    else
    {
        printf("[CONNECTED] Link dropped. Reconnecting...\n");

        *p_next_state = WIFI_CONNECTING;
    }

    return WIFI_STATUS_OK;
}

/**
 * @brief Recover from the WIFI_ERROR state.
 *
 * @param[in] input Input value. Ignored.
 * @param[out] p_next_state Pointer receiving WIFI_INIT.
 *
 * @return WIFI_STATUS_OK.
 */
static uint32_t wifi_state_error(uint8_t input,
                                 WIFI_STATE *p_next_state)
{
    (void)input;

    printf("[ERROR] Recovery. Restarting -> INIT\n");

    *p_next_state = WIFI_INIT;

    return WIFI_STATUS_OK;
}

/**
 * @brief Return a human-readable WiFi state name.
 *
 * @param[in] state WiFi state.
 *
 * @return Constant state-name string.
 */
static const char *wifi_state_name(WIFI_STATE state)
{
    const char *p_name;

    switch (state)
    {
        case WIFI_INIT:
            p_name = "WIFI_INIT";
            break;

        case WIFI_CONNECTING:
            p_name = "WIFI_CONNECTING";
            break;

        case WIFI_CONNECTED:
            p_name = "WIFI_CONNECTED";
            break;

        case WIFI_ERROR:
            p_name = "WIFI_ERROR";
            break;

        case WIFI_MAX_STATES:
        default:
            p_name = "WIFI_INVALID";
            break;
    }

    return p_name;
}

/**
 * @brief Execute one WiFi FSM step.
 *
 * @param[in] input Current input value.
 * @param[in,out] p_state Pointer to current state.
 *
 * @return WIFI_STATUS_OK on success or WIFI_STATUS_ERROR on error.
 */
static uint32_t RunStateMachine(uint8_t input,
                                WIFI_STATE *p_state)
{
    uint32_t status = WIFI_STATUS_OK;
    WIFI_STATE next_state;
    wifi_handler_t p_handler = NULL;

    if (p_state == NULL)
    {
        printf("[FSM ERROR] NULL state pointer.\n");
        status = WIFI_STATUS_ERROR;
    }
    else if ((*p_state < WIFI_INIT) ||
             (*p_state >= WIFI_MAX_STATES))
    {
        printf("[FSM ERROR] Invalid state. Recovering to WIFI_INIT.\n");

        *p_state = WIFI_INIT;
        status = WIFI_STATUS_ERROR;
    }
    else
    {
        p_handler = WifiFSM[*p_state];

        if (p_handler == NULL)
        {
            printf(
                "[FSM ERROR] NULL state handler. "
                "Recovering to WIFI_INIT.\n");

            *p_state = WIFI_INIT;
            status = WIFI_STATUS_ERROR;
        }
        else
        {
            next_state = *p_state;

            status = p_handler(input, &next_state);

            if (status == WIFI_STATUS_OK)
            {
                *p_state = next_state;
            }
            else
            {
                *p_state = WIFI_INIT;
            }
        }
    }

    return status;
}

/**
 * @brief Program entry point and FSM demonstration.
 *
 * The FSM performs one startup INIT tick and then processes the required
 * input sequence:
 *
 * {0, 0, 1, 0, 0, 0, 0, 1}
 *
 * @return 0 when the FSM test completes successfully, otherwise 1.
 */
int main(void)
{
    static const uint8_t input_sequence[] =
    {
        0U,
        0U,
        1U,
        0U,
        0U,
        0U,
        0U,
        1U
    };

    WIFI_STATE state = WIFI_INIT;
    uint32_t index;
    uint32_t status;
    const uint32_t input_count =
        (uint32_t)(sizeof(input_sequence) / sizeof(input_sequence[0]));

    /*
     * Startup FSM tick.
     *
     * WIFI_INIT transitions unconditionally to WIFI_CONNECTING.
     * This startup tick is separate from the required test sequence.
     */
    printf(
        "[Step 0] State: %-15s | input=0\n",
        wifi_state_name(state));

    status = RunStateMachine(0U, &state);

    if (status != WIFI_STATUS_OK)
    {
        return 1;
    }

    printf("\n");

    /*
     * Process the required eight-element input sequence.
     */
    for (index = 0U; index < input_count; index++)
    {
        printf(
            "[Step %u] State: %-15s | input=%u\n",
            (unsigned int)(index + 1U),
            wifi_state_name(state),
            (unsigned int)input_sequence[index]);

        status = RunStateMachine(input_sequence[index], &state);

        if (status != WIFI_STATUS_OK)
        {
            return 1;
        }

        printf("\n");
    }

    return 0;
}