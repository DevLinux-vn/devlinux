/**
 * @file wifi_fsm.c
 * @brief Implementation of Scalable WiFi Connection State Machine.
 */

#include "wifi_fsm.h"
#include <stdio.h>

/**
 * @brief Handler for WIFI_INIT state.
 * @param[in]     input        Hardware input event (unused).
 * @param[in,out] p_next_state Pointer to next state variable.
 * @return 0 on success.
 */
static uint32_t wifi_state_init(uint8_t input, WIFI_STATE *p_next_state)
{
    (void)input;
    printf("[INIT] Initializing... -> CONNECTING\n");
    *p_next_state = WIFI_CONNECTING;
    return 0U;
}

/**
 * @brief Handler for WIFI_CONNECTING state with internal retry counter.
 * @param[in]     input        Hardware input event (1 = link success, 0 = failure).
 * @param[in,out] p_next_state Pointer to next state variable.
 * @return 0 on success.
 */
static uint32_t wifi_state_connecting(uint8_t input, WIFI_STATE *p_next_state)
{
    static uint32_t s_retry_count = 0U;

    if (1U == input) {
        s_retry_count = 0U;
        printf("[CONNECTING] Connected! Retry count reset.\n");
        *p_next_state = WIFI_CONNECTED;
    } else {
        s_retry_count++;
        if (s_retry_count >= 3U) {
            s_retry_count = 0U;
            printf("[CONNECTING] Attempt 3 failed. -> ERROR\n");
            *p_next_state = WIFI_ERROR;
        } else {
            printf("[CONNECTING] Attempt %u failed. Retrying...\n", s_retry_count);
            *p_next_state = WIFI_CONNECTING;
        }
    }
    return 0U;
}

/**
 * @brief Handler for WIFI_CONNECTED state.
 * @param[in]     input        Hardware input event (1 = link maintained, 0 = dropped).
 * @param[in,out] p_next_state Pointer to next state variable.
 * @return 0 on success.
 */
static uint32_t wifi_state_connected(uint8_t input, WIFI_STATE *p_next_state)
{
    if (1U == input) {
        printf("[CONNECTED] Online.\n");
        *p_next_state = WIFI_CONNECTED;
    } else {
        printf("[CONNECTED] Link dropped. Reconnecting...\n");
        *p_next_state = WIFI_CONNECTING;
    }
    return 0U;
}

/**
 * @brief Handler for WIFI_ERROR state.
 * @param[in]     input        Hardware input event (unused).
 * @param[in,out] p_next_state Pointer to next state variable.
 * @return 0 on success.
 */
static uint32_t wifi_state_error(uint8_t input, WIFI_STATE *p_next_state)
{
    (void)input;
    printf("[ERROR] Recovery. Restarting -> INIT\n");
    *p_next_state = WIFI_INIT;
    return 0U;
}

/**
 * @brief Function pointer table mapped by state enum, stored in Flash (.rodata).
 */
static const wifi_handler_t WifiFSM[WIFI_MAX_STATES] = {
    [WIFI_INIT]       = wifi_state_init,
    [WIFI_CONNECTING] = wifi_state_connecting,
    [WIFI_CONNECTED]  = wifi_state_connected,
    [WIFI_ERROR]      = wifi_state_error
};

uint32_t RunStateMachine(uint8_t input, WIFI_STATE *p_state)
{
    /* MISRA Directive 4.11 / CERT EXP34-C: Guard state pointer against NULL */
    if (NULL == p_state) {
        return 0xFFU;
    }

    /* CERT ARR30-C: Bounds check before indexing table */
    if (*p_state >= WIFI_MAX_STATES) {
        *p_state = WIFI_INIT; /* Error recovery */
        return 0xFFU;
    }

    wifi_handler_t p_handler = WifiFSM[*p_state];
    /* MISRA Directive 4.11: Guard function pointer against NULL */
    if (p_handler != NULL) {
        return p_handler(input, p_state);
    }

    return 0xFFU;
}

const char *WiFi_GetStateName(WIFI_STATE state)
{
    const char *p_name = "UNKNOWN";
    switch (state) {
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
        default:
            break;
    }
    return p_name;
}
