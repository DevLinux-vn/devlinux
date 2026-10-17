#ifndef SORTED_LIST_H
#define SORTED_LIST_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define NULL_POINTER    (0U)
#define EMPTY_LIST      (0U)

/**
 * @brief Structure representing a sensor event node.
 *
 * Stores the timestamp, sensor identifier, sensor reading, and a pointer
 * to the next event node in the singly-linked list.
 */
typedef struct event_node {
    uint32_t timestamp;
    uint8_t  sensor_id;
    int16_t  reading;
    struct event_node *p_next;
} event_node_t;

/**
 * @brief Create and initialize a new event node.
 *
 * @param timestamp Timestamp of the sensor event.
 * @param sensor_id Identifier of the sensor that generated the event.
 * @param reading   Sensor reading value.
 *
 * @return Pointer to the newly created event node, or NULL if memory
 *         allocation fails.
 */
event_node_t *create_event_node(uint32_t timestamp,
                                uint8_t sensor_id,
                                int16_t reading);

/**
 * @brief Insert a new event into the list in sorted order.
 *
 * The event is inserted according to its timestamp while ensuring that
 * the list does not exceed the specified maximum capacity.
 *
 * @param pp_head       Address of the list head pointer.
 * @param p_new        Pointer to the new event node to be inserted.
 * @param max_capacity Maximum number of nodes allowed in the list.
 *
 * @return true if the event was successfully inserted, otherwise false.
 */
bool sorted_insert(event_node_t **pp_head,
                   event_node_t *p_new,
                   uint32_t max_capacity);

/**
 * @brief Delete the first event matching the specified sensor ID.
 *
 * @param pp_head   Address of the list head pointer.
 * @param sensor_id Identifier of the sensor whose event should be deleted.
 *
 * @return true if a matching event was found and deleted, otherwise false.
 */
bool delete_event_by_id(event_node_t **pp_head, uint8_t sensor_id);

/**
 * @brief Get the number of events in the list.
 *
 * @param p_head Pointer to the head of the event list.
 *
 * @return Number of event nodes in the list.
 */
uint32_t get_list_length(const event_node_t *p_head);

/**
 * @brief Print all events in the list.
 *
 * @param p_head Pointer to the head of the event list.
 */
void print_events(const event_node_t *p_head);

/**
 * @brief Free all nodes in the event list.
 *
 * Releases the memory allocated for every node and sets the list head
 * pointer to NULL.
 *
 * @param pp_head Address of the list head pointer.
 */
void free_list(event_node_t **pp_head);

#endif /* SORTED_LIST_H */