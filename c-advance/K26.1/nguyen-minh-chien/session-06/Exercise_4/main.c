#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

/**
 * @brief Traffic light states. NUM_STATES is a sentinel marking the
 *        total valid state count — not a real state.
 */
typedef enum {
    RED =0,
    GREEN,
    YELLOW,
    NUM_STATES
}traffic_state_t;

void run_traffic_light_bad(traffic_state_t *p_state, uint32_t tick);
/**
 * @brief Original switch-case traffic light controller (kept for
 *        structural comparison — not called from main()).
 * @param[in,out] p_state Pointer to current state.
 * @param[in]     tick    Current tick number.
 */
void run_traffic_light_bad(traffic_state_t *p_state, uint32_t tick) {
    switch (*p_state) {
        case RED:
            printf("[RED]    Tick %u — Stop! Holding for 3 ticks.\n", tick);
            if (tick % 3U == 0U) { *p_state = GREEN; }
            break;
        case GREEN:
            printf("[GREEN]  Tick %u — Go!  Holding for 3 ticks.\n", tick);
            if (tick % 3U == 0U) { *p_state = YELLOW; }
            break;
        case YELLOW:
            printf("[YELLOW] Tick %u — Slow down!\n", tick);
            *p_state = RED;
            break;
        default: break;
    }
}

/**
 * @brief Uniform function-pointer signature for all traffic states.
 */
typedef void (*traffic_handler_t)(uint32_t tick, traffic_state_t *p_next_state);

/**
 * @brief Handle the RED state: stop, hold, transition to GREEN every 3 ticks.
 * @param[in]  tick          Current tick number.
 * @param[out] p_next_state  Written with the next state.
 */
static void State_Red   (uint32_t tick, traffic_state_t *p_next_state);
static void State_Green (uint32_t tick, traffic_state_t *p_next_state);
static void State_Yellow(uint32_t tick, traffic_state_t *p_next_state);

static const traffic_handler_t TrafficFSM[NUM_STATES] =
{
    [RED]    = State_Red,
    [GREEN]  = State_Green,
    [YELLOW] = State_Yellow
};


static void State_Red   (uint32_t tick, traffic_state_t *p_next_state){
    *p_next_state = RED;
    (void)printf("[RED]    Tick %u — Stop! Holding for 3 ticks.\n",tick);
    if(tick %3U == 0U) {*p_next_state = GREEN;}
}

static void State_Green (uint32_t tick, traffic_state_t *p_next_state){
    *p_next_state = GREEN;
    (void)printf("[GREEN]  Tick %u — Go!  Holding for 3 ticks.\n", tick);
    if(tick %3U == 0U) {*p_next_state = YELLOW;}
}

static void State_Yellow(uint32_t tick, traffic_state_t *p_next_state){
    (void)printf("[YELLOW] Tick %u — Slow down!\n", tick);
    *p_next_state = RED;
}

/**
 * @brief Run one tick of the traffic light FSM via table dispatch.
 * @param[in]     tick     Current tick number.
 * @param[in,out] p_state  Pointer to current state; updated by dispatch.
 */
static void RunTrafficFSM(uint32_t tick, traffic_state_t *p_state){
    if (*p_state >= NUM_STATES)
    {
        (void)printf("[FSM] Invalid state!\n");
        return;
    }
    if (TrafficFSM[*p_state] == NULL)
    {
        (void)printf("[FSM] Invalid state (null)!\n");
        return;
    }
    TrafficFSM[*p_state](tick, p_state);

}

/**
 * @brief Program entry point. Runs the FSM for 10 ticks.
 * @return 0 on success.
 */
int main (void){
    traffic_state_t state = RED;
    for (uint32_t tick = 1U; tick <= 10U; tick++)
    {
        RunTrafficFSM(tick, &state);
    }
    return 0;
}

