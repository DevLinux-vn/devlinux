#include "sorted_list.h"

event_node_t* create_event_node(uint32_t timestamp, uint8_t sensor_id, int16_t reading) {
    event_node_t *p_node = malloc(sizeof(*p_node));

    if (NULL == p_node) {
        return NULL;
    }

    p_node->timestamp = timestamp;
    p_node->sensor_id = sensor_id;
    p_node->reading = reading;
    p_node->p_next = NULL;

    return p_node;
}

bool sorted_insert(event_node_t **pp_head, event_node_t *p_new, uint32_t max_capacity) {
    if ((NULL == p_new) || (NULL == pp_head)) {
        return false;
    }

    if (NULL == *pp_head) {
        *pp_head = p_new;
        p_new->p_next = NULL;
    }
    else if (p_new->timestamp < (*pp_head)->timestamp) {
        p_new->p_next = *pp_head;
        *pp_head = p_new;
    }
    else {
        event_node_t *p_previous = NULL;
        event_node_t *p_current = *pp_head;

        while ((NULL != p_current) && (p_new->timestamp > p_current->timestamp)) {
            p_previous = p_current;
            p_current = p_current->p_next;
        }

        p_new->p_next = p_current;
        p_previous->p_next = p_new;
    }

    uint32_t count = 0U;
    event_node_t *p_current = *pp_head;
    
    while (p_current != NULL) {
        count++;
        p_current = p_current->p_next;
    }

    if (count > max_capacity) {
        event_node_t *p_old = *pp_head;
        *pp_head = p_old->p_next;

        free(p_old);
    }

    return true;
}


bool delete_event_by_id(event_node_t **pp_head, uint8_t sensor_id) {
    if (NULL == pp_head) {
        return NULL_POINTER;
    }

    if (NULL == *pp_head) {
        return EMPTY_LIST;
    }

    event_node_t *p_current = *pp_head;
    event_node_t *p_previous = NULL;

    while ((NULL != p_current) && p_current->sensor_id != sensor_id) {
        p_previous = p_current;
        p_current = p_current->p_next;
    }

    if (NULL == p_current) {
        return false;
    }

    if (NULL == p_previous) {
        *pp_head = p_current->p_next;
    }
    else {
        p_previous->p_next = p_current->p_next;
    }

    free(p_current);

    return true;
}

uint32_t get_list_length(const event_node_t *p_head) {
    if (NULL == p_head) {
        return NULL_POINTER;
    }

    uint32_t count = 0U;
    const event_node_t *p_current = p_head;

    while (NULL != p_current) {
        count++;
        p_current = p_current->p_next;
    }

    return count;
}

void print_events(const event_node_t *p_head) {
    if (NULL == p_head) {
        return;
    }

    const event_node_t *p_current = p_head;

    while (NULL != p_current) {
        printf("[t=%-3u]  Sensor ID: %u, Reading: %d\n", p_current->timestamp, p_current->sensor_id, p_current->reading);
        p_current = p_current->p_next;
    }

    return;
}

void free_list(event_node_t **pp_head) {
    if (NULL == pp_head) {
        return;
    }

    event_node_t *p_current = *pp_head;
    event_node_t *p_next = NULL;

    while (NULL != p_current) {
        p_next = p_current->p_next;
        free(p_current);
        p_current = p_next;
    }

    *pp_head = NULL;

    return;
}