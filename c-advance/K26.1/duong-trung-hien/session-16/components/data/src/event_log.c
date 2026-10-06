#include "event_log.h"
#include "common_types.h"
#include "contract.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static event_node_t s_pool[EVENT_LOG_MAX_ENTRIES];
static uint32_t s_pool_index;
static event_node_t *s_p_head;
static uint32_t s_event_count;

static const char *event_log_get_type_name(event_type_t type) {
  static const char *const event_names[EVENT_NUM_TYPES] = {
      "ALERT_TEMP_HIGH", "ALERT_TEMP_LOW", "ALERT_HUMI_HIGH", "SENSOR_ERROR",
      "SYSTEM_RESET"};

  if (type >= EVENT_NUM_TYPES) {
    return "UNKNOWN";
  }

  return event_names[type];
}

static event_node_t *event_log_get_free_node(void) {
  event_node_t *p_node;

  if (s_pool_index < EVENT_LOG_MAX_ENTRIES) {
    p_node = &s_pool[s_pool_index];
    s_pool_index++;
    return p_node;
  }

  /*
   * Pool is full. Find the oldest node, which is the tail
   * of the list because the list is sorted newest first.
   */
  p_node = s_p_head;

  if (p_node == NULL) {
    return NULL;
  }

  if (p_node->p_next == NULL) {
    s_p_head = NULL;
    return p_node;
  }

  while (p_node->p_next->p_next != NULL) {
    p_node = p_node->p_next;
  }

  {
    event_node_t *p_oldest = p_node->p_next;

    p_node->p_next = NULL;

    return p_oldest;
  }
}

void event_log_init(void) {
  uint32_t index;

  s_pool_index = 0U;
  s_p_head = NULL;
  s_event_count = 0U;

  for (index = 0U; index < EVENT_LOG_MAX_ENTRIES; index++) {
    s_pool[index].type = EVENT_SYSTEM_RESET;
    s_pool[index].timestamp = 0U;
    s_pool[index].value = 0;
    s_pool[index].p_next = NULL;
  }
}

/* NOLINTNEXTLINE(bugprone-easily-swappable-parameters) */
status_t event_log_add(event_type_t type, uint32_t timestamp, int16_t value) {
  event_node_t *p_new;
  event_node_t *p_current;

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  REQUIRE(type < EVENT_NUM_TYPES);

  p_new = event_log_get_free_node();

  if (p_new == NULL) {
    return STATUS_ERR_INVALID_PARAM;
  }

  p_new->type = type;
  p_new->timestamp = timestamp;
  p_new->value = value;
  p_new->p_next = NULL;

  /*
   * Insert at the head when the list is empty or the new
   * event is newer than the current head.
   */
  if ((s_p_head == NULL) || (timestamp >= s_p_head->timestamp)) {
    p_new->p_next = s_p_head;
    s_p_head = p_new;
  } else {
    p_current = s_p_head;

    while ((p_current->p_next != NULL) &&
           (p_current->p_next->timestamp >= timestamp)) {
      p_current = p_current->p_next;
    }

    p_new->p_next = p_current->p_next;
    p_current->p_next = p_new;
  }

  if (s_event_count < EVENT_LOG_MAX_ENTRIES) {
    s_event_count++;
  }

  return STATUS_OK;
}

uint32_t event_log_get_count(void) { return s_event_count; }

void event_log_print_all(void) {
  const event_node_t *p_current = s_p_head;

  (void)printf("Event Log:\n");

  while (p_current != NULL) {
    (void)printf(
        "  [tick=%3lu] %s: value=%d\n", (unsigned long)p_current->timestamp,
        event_log_get_type_name(p_current->type), (int)p_current->value);

    p_current = p_current->p_next;
  }

  (void)printf("  Total events: %lu\n", (unsigned long)s_event_count);
}