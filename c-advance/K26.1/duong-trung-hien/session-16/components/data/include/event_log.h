#ifndef EVENT_LOG_H
#define EVENT_LOG_H

#include <stdint.h>

#include "common_types.h"

#define EVENT_LOG_MAX_ENTRIES (32U)

/**
 * @brief Event types stored in the event log.
 */
typedef enum {
  EVENT_ALERT_TEMP_HIGH = 0U,
  EVENT_ALERT_TEMP_LOW,
  EVENT_ALERT_HUMI_HIGH,
  EVENT_SENSOR_ERROR,
  EVENT_SYSTEM_RESET,
  EVENT_NUM_TYPES
} event_type_t;

/**
 * @brief Intrusive singly linked event node.
 */
typedef struct event_node {
  event_type_t type;
  uint32_t timestamp;
  int16_t value;
  struct event_node *p_next;
} event_node_t;

/**
 * @brief Initialize the event log and reset its memory pool.
 */
void event_log_init(void);

/**
 * @brief Add an event to the event log.
 *
 * Events are sorted by timestamp in descending order.
 * When the log is full, the oldest event is evicted.
 *
 * @param type Event type.
 * @param timestamp Event timestamp.
 * @param value Associated event value.
 *
 * @return STATUS_OK on success, error status otherwise.
 */
status_t event_log_add(event_type_t type, uint32_t timestamp, int16_t value);

/**
 * @brief Get the current number of events.
 *
 * @return Number of stored events.
 */
uint32_t event_log_get_count(void);

/**
 * @brief Print all events from newest to oldest.
 */
void event_log_print_all(void);

#endif /* EVENT_LOG_H */