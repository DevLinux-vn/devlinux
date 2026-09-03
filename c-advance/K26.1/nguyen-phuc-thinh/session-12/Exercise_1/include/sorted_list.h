/**
 * @file sorted_list.h
 * @brief Definition of sorted linked list for sensor events.
 */
#ifndef SORTED_LIST_H
#define SORTED_LIST_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Node structure holding sensor data and pointer to next node.
 */
typedef struct event_node 
{
    uint32_t timestamp;
    uint8_t sensor_id;
    int16_t reading;
    struct event_node *p_next;
} event_node_t;

/**
 * @brief Allocates and initializes a new event node.
 * @param timestamp Time of the event.
 * @param sensor_id ID of the sensor.
 * @param reading Sensor value.
 * @return Pointer to the allocated node or NULL if allocation fails.
 */
event_node_t* create_event_node(uint32_t timestamp, uint8_t sensor_id, int16_t reading);

/**
 * @brief Inserts a node into the list in ascending timestamp order, enforcing capacity.
 * @param pp_head Double pointer to the head of the list.
 * @param p_new Pointer to the new node to insert.
 * @param max_capacity Maximum allowed nodes in the list.
 * @return true if inserted successfully, false otherwise.
 */
bool sorted_insert(event_node_t **pp_head, event_node_t *p_new, uint32_t max_capacity);

/**
 * @brief Deletes the first node matching the specified sensor ID.
 * @param pp_head Double pointer to the head of the list.
 * @param sensor_id Target sensor ID to delete.
 * @return true if deleted successfully, false if not found.
 */
bool delete_event_by_id(event_node_t **pp_head, uint8_t sensor_id);

/**
 * @brief Calculates the total number of nodes in the list.
 * @param p_head Pointer to the head of the list.
 * @return The number of nodes.
 */
uint32_t get_list_length(const event_node_t *p_head);

/**
 * @brief Traverses and prints all events in the list.
 * @param p_head Pointer to the head of the list.
 */
void print_events(const event_node_t *p_head);

/**
 * @brief Safely frees all nodes in the list and nulls the head pointer.
 * @param pp_head Double pointer to the head of the list.
 */
void free_list(event_node_t **pp_head);

#endif /* SORTED_LIST_H */