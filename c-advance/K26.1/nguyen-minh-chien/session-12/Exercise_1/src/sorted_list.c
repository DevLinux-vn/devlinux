/**
 * @file    sorted_list.c
 * @brief   Implementation of the bounded-capacity, timestamp-sorted
 *          sensor event linked list.
 */
#include "sorted_list.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Allocate and initialize a new, unlinked event node.
 */
event_node_t *create_event_node(uint32_t timestamp, uint8_t sensor_id, int16_t reading)
{
    event_node_t *p_node = malloc(sizeof(event_node_t));

    if (p_node != NULL)
    {
        p_node->timestamp = timestamp;
        p_node->sensor_id = sensor_id;
        p_node->reading = reading;
        p_node->p_next = NULL;
    }

    return p_node;
}

/**
 * @brief Insert a node in ascending-timestamp order, evicting the
 *        oldest node on overflow.
 */
bool sorted_insert(event_node_t **pp_head, event_node_t *p_new, uint32_t max_capacity)
{
    /* CERT EXP34-C: verify both pointers before any dereference. */
    if ((pp_head == NULL) || (p_new == NULL))
    {
        return false;
    }

    if ((*pp_head == NULL) || (p_new->timestamp <= (*pp_head)->timestamp))
    {
        /* Insert at head: empty list, or p_new is now the oldest. */
        p_new->p_next = *pp_head;
        *pp_head = p_new;
    }
    else
    {
        /* Walk until the node just before the correct insertion
         * point: the last node whose next-neighbor's timestamp is
         * NOT smaller than p_new's (or the tail, if p_new belongs
         * at the end). */
        event_node_t *p_curr = *pp_head;

        while ((p_curr->p_next != NULL) && (p_curr->p_next->timestamp < p_new->timestamp))
        {
            p_curr = p_curr->p_next;
        }

        p_new->p_next = p_curr->p_next;
        p_curr->p_next = p_new;
    }

    /* Enforce bounded capacity: if the list is now too long, evict
     * (free) the node with the smallest timestamp, i.e. the head.
     * This is the "medical monitor bug" guard -- the list can never
     * grow past max_capacity nodes, no matter how many events are
     * inserted. */
    if ((max_capacity > 0U) && (get_list_length(*pp_head) > max_capacity))
    {
        event_node_t *p_oldest = *pp_head;

        *pp_head = p_oldest->p_next;
        free(p_oldest);
    }

    return true;
}

/**
 * @brief Find, unlink, and free the first node matching sensor_id.
 */
bool delete_event_by_id(event_node_t **pp_head, uint8_t sensor_id)
{
    /* CERT EXP34-C: verify pp_head and *pp_head before use. */
    if ((pp_head == NULL) || (*pp_head == NULL))
    {
        return false;
    }

    event_node_t *p_prev = NULL;
    event_node_t *p_curr = *pp_head;

    while ((p_curr != NULL) && (p_curr->sensor_id != sensor_id))
    {
        p_prev = p_curr;
        p_curr = p_curr->p_next;
    }

    if (p_curr == NULL)
    {
        /* Reached the end of the list without a match. */
        return false;
    }

    if (p_prev == NULL)
    {
        /* Deleting the head node (covers the single-node-list case
         * too, where p_curr->p_next is NULL and *pp_head correctly
         * becomes NULL). */
        *pp_head = p_curr->p_next;
    }
    else
    {
        /* Deleting a middle or tail node: unlink by pointing the
         * previous node's p_next past p_curr. */
        p_prev->p_next = p_curr->p_next;
    }

    free(p_curr);
    return true;
}

/**
 * @brief Count the number of nodes in the list.
 */
uint32_t get_list_length(const event_node_t *p_head)
{
    uint32_t count = 0U;
    const event_node_t *p_curr = p_head;

    while (p_curr != NULL)
    {
        count++;
        p_curr = p_curr->p_next;
    }

    return count;
}

/**
 * @brief Print every event in the list, head to tail.
 */
void print_events(const event_node_t *p_head)
{
    char ts_label[16];
    const event_node_t *p_curr = p_head;

    while (p_curr != NULL)
    {
        /* C11 Annex K's snprintf_s is optional and unavailable on
         * glibc/Linux targets; plain snprintf() with an explicit
         * buffer size is the portable, still-bounded-write choice
         * here. */
        // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        (void)snprintf(ts_label, sizeof(ts_label), "[t=%u]", p_curr->timestamp);
        (void)printf("%-7s Sensor ID: %u, Reading: %d\n",
                     ts_label,
                     (unsigned int)p_curr->sensor_id,
                     p_curr->reading);
        p_curr = p_curr->p_next;
    }
}

/**
 * @brief Free every node in the list and reset the head to NULL.
 */
void free_list(event_node_t **pp_head)
{
    if ((pp_head == NULL) || (*pp_head == NULL))
    {
        return;
    }

    event_node_t *p_curr = *pp_head;

    while (p_curr != NULL)
    {
        /* Save p_next BEFORE freeing p_curr: reading p_curr->p_next
         * after free(p_curr) would be use-after-free (undefined
         * behavior). This is the single most important line in this
         * function. */
        event_node_t *p_next = p_curr->p_next;

        free(p_curr);
        p_curr = p_next;
    }

    /* Always leave the caller's head pointer in a known-safe state,
     * never dangling. */
    *pp_head = NULL;
}