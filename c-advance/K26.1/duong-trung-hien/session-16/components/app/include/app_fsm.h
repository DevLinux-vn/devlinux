#ifndef APP_FSM_H
#define APP_FSM_H

#include "common_types.h"

/**
 * @brief System events used by the application FSM.
 */
typedef enum {
  EVT_INIT_OK = 0U,
  EVT_INIT_FAIL,
  EVT_CMD_START,
  EVT_CMD_STOP,
  EVT_CMD_RESET,
  EVT_CMD_QUIT,
  EVT_ALERT_TRIGGERED,
  EVT_ALERT_ACK,
  EVT_SENSOR_FAIL,
  EVT_NUM_EVENTS
} system_event_t;

/**
 * @brief Function pointer type for FSM state handlers.
 *
 * @param[in] event Event received by the current state.
 *
 * @return Next FSM state.
 */
typedef system_state_t (*state_handler_t)(system_event_t event);

/**
 * @brief Execute one FSM transition.
 *
 * @param[in] current_state Current FSM state.
 * @param[in] event Event to process.
 *
 * @return Next FSM state.
 *         The current state is returned when the state or event is invalid.
 */
system_state_t fsm_run(system_state_t current_state, system_event_t event);

#endif /* APP_FSM_H */