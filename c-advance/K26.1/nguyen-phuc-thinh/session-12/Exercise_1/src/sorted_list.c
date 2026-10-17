#include "sorted_list.h"
#include <stdio.h>
#include <stdlib.h>

event_node_t* create_event_node(uint32_t timestamp, uint8_t sensor_id, int16_t reading)
{
    event_node_t *p_node = (event_node_t *)malloc(sizeof(event_node_t));
    if (p_node != NULL)
    {
        p_node->timestamp = timestamp;
        p_node->sensor_id = sensor_id;
        p_node->reading = reading;
        p_node->p_next = NULL;
    }
    return p_node;
}

uint32_t get_list_length(const event_node_t *p_head)
{
    uint32_t count = 0;
    const event_node_t *p_curr = p_head;
    while (p_curr != NULL)
    {
        count++;
        p_curr = p_curr->p_next;
    }
    return count;
}

bool sorted_insert(event_node_t **pp_head, event_node_t *p_new, uint32_t max_capacity)
{
    if ((pp_head == NULL) || (p_new == NULL) || (max_capacity == 0))
    {
        return false;
    }

    event_node_t *p_curr = *pp_head;
    event_node_t *p_prev = NULL;

    while ((p_curr != NULL) && (p_curr->timestamp <= p_new->timestamp))
    {
        p_prev = p_curr;
        p_curr = p_curr->p_next;
    }

    if (p_prev == NULL)
    {
        p_new->p_next = *pp_head;
        *pp_head = p_new;
    }
    else
    {
        p_new->p_next = p_curr;
        p_prev->p_next = p_new;
    }

    if (get_list_length(*pp_head) > max_capacity)
    {
        event_node_t *p_evict = *pp_head;
        *pp_head = (*pp_head)->p_next;
        printf("    Inserted: [t=%u, ID=%u, Val=%d] -> Capacity exceeded! Evicted oldest [t=%u].\n",
               p_new->timestamp, p_new->sensor_id, p_new->reading, p_evict->timestamp);
        free(p_evict);
    }
    else
    {
        printf("    Inserted: [t=%u, ID=%u, Val=%d]\n", p_new->timestamp, p_new->sensor_id, p_new->reading);
    }

    return true;
}

bool delete_event_by_id(event_node_t **pp_head, uint8_t sensor_id)
{
    if ((pp_head == NULL) || (*pp_head == NULL))
    {
        return false;
    }

    event_node_t *p_curr = *pp_head;
    event_node_t *p_prev = NULL;

    while (p_curr != NULL)
    {
        if (p_curr->sensor_id == sensor_id)
        {
            if (p_prev == NULL)
            {
                *pp_head = p_curr->p_next;
            }
            else
            {
                p_prev->p_next = p_curr->p_next;
            }
            free(p_curr);
            return true;
        }
        p_prev = p_curr;
        p_curr = p_curr->p_next;
    }
    return false;
}

void print_events(const event_node_t *p_head)
{
    const event_node_t *p_curr = p_head;
    while (p_curr != NULL)
    {
        printf("[t=%u]  Sensor ID: %u, Reading: %d\n", p_curr->timestamp, p_curr->sensor_id, p_curr->reading);
        p_curr = p_curr->p_next;
    }
}

void free_list(event_node_t **pp_head)
{
    if ((pp_head == NULL) || (*pp_head == NULL))
    {
        return;
    }

    event_node_t *p_curr = *pp_head;
    while (p_curr != NULL)
    {
        event_node_t *p_next = p_curr->p_next;
        free(p_curr);
        p_curr = p_next;
    }
    *pp_head = NULL;
}