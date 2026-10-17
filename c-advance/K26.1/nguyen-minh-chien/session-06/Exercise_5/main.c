#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

typedef enum {
    WIFI_INIT =0,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_ERROR,
    WIFI_MAX_STATES
}wifi_state_t;

typedef uint32_t (*wifi_handler_t)(uint8_t input, wifi_state_t *p_next_state);

static uint32_t wifi_state_init (uint8_t input, wifi_state_t *p_next_state);
static uint32_t wifi_state_connecting (uint8_t input, wifi_state_t *p_next_state);
static uint32_t wifi_state_connected (uint8_t input, wifi_state_t *p_next_state);
static uint32_t wifi_state_error (uint8_t input, wifi_state_t *p_next_state);

static const wifi_handler_t WifiFSM[WIFI_MAX_STATES] =
{
    [WIFI_INIT] = wifi_state_init,
    [WIFI_CONNECTING] = wifi_state_connecting,
    [WIFI_CONNECTED] = wifi_state_connected,
    [WIFI_ERROR] = wifi_state_error
};

static uint32_t wifi_state_init (uint8_t input, wifi_state_t *p_next_state){
    (void)input;
    *p_next_state = WIFI_CONNECTING;
    (void)printf("[INIT] Initializing... -> CONNECTING\n");
    return 0;
}

static uint32_t wifi_state_connecting (uint8_t input, wifi_state_t *p_next_state){
    static uint32_t s_retry_count = 0U;
    if(input == 1U){
        s_retry_count= 0U;
        *p_next_state = WIFI_CONNECTED;
        (void)printf("[CONNECTING] Connected! Retry count reset.\n");
    }
    else
    {
        s_retry_count++;
        if (s_retry_count >= 3U)
        {
            (void)printf("[CONNECTING] Attempt %u failed. -> ERROR\n", s_retry_count);
            s_retry_count = 0U;
            *p_next_state = WIFI_ERROR;
        }
        else
        {
            (void)printf("[CONNECTING] Attempt %u failed. Retrying...\n", s_retry_count);
            *p_next_state = WIFI_CONNECTING;
        }
    }
    return 0U;
}

static uint32_t wifi_state_connected (uint8_t input, wifi_state_t *p_next_state){
    if (input == 1U){
        *p_next_state = WIFI_CONNECTED;
        (void)printf("[CONNECTED] Link is online.\n");
    }
    else{
        *p_next_state = WIFI_CONNECTING;
        (void)printf("[CONNECTED] Link dropped. Reconnecting...\n");
    }
    return 0U;
}

static uint32_t wifi_state_error(uint8_t input, wifi_state_t *p_next_state)
{
    (void)input;  
    *p_next_state = WIFI_INIT;
    (void)printf("[ERROR] Recovery. Restarting -> INIT\n");
    return 0U;
}

/**
 * @brief Run one step of the WiFi FSM.
 * @param[in]     input     Hardware/event input (1 = link up, 0 = link down/fail).
 * @param[in,out] p_state   Pointer to the current state; updated on transition.
 * @return 0 on success, 0xFF on invalid state.
 */
uint32_t RunStateMachine(uint8_t input, wifi_state_t *p_state){
    uint32_t result;
    if (*p_state >= WIFI_MAX_STATES)
    {
        (void)printf("[FSM] FATAL: Invalid state!\n");
        *p_state = WIFI_INIT;
        result = 0xFFU;
    }
    else if (WifiFSM[*p_state] == NULL)
    {
        (void)printf("[FSM] FATAL: NULL handler!\n");
        *p_state = WIFI_INIT;
        result = 0xFFU;
    }
    else
    {
        result = WifiFSM[*p_state](input, p_state);
    }
    return result;
}

/**
 * @brief Look up the human-readable name of a WiFi state.
 * @param[in] state The state to name.
 * @return Pointer to a static string literal naming the state.
 */
static const char *StateName(wifi_state_t state)
{
    static const char * const names[WIFI_MAX_STATES] =
    {
        [WIFI_INIT]       = "WIFI_INIT",
        [WIFI_CONNECTING] = "WIFI_CONNECTING",
        [WIFI_CONNECTED]  = "WIFI_CONNECTED",
        [WIFI_ERROR]      = "WIFI_ERROR"
    };
    return names[state];
}

/**
 * @brief Program entry point. Drives the WiFi FSM through a fixed input
 *        sequence and prints each state transition, including the
 *        automatic ERROR -> INIT recovery step.
 * @return 0 on success.
 */
int main(void)
{
    static const uint8_t inputs[] = {0U, 0U, 0U, 1U, 0U, 0U, 0U, 0U};
    wifi_state_t state = WIFI_INIT;
    uint32_t step;
    uint32_t num_inputs = (uint32_t)(sizeof(inputs) / sizeof(inputs[0]));

    for (step = 0U; step < num_inputs; step++)
    {
        (void)printf("[Step %u] State: %-15s | input=%u\n", step, StateName(state), inputs[step]);
        (void)RunStateMachine(inputs[step], &state);
        (void)printf("\n");
    }

    if (state == WIFI_ERROR)
    {
        (void)printf("[Step %u] (auto) State: %-15s | input=%u\n", step, StateName(state), inputs[step - 1U]);
        (void)RunStateMachine(inputs[step - 1U], &state);
    }

    return 0;
}
