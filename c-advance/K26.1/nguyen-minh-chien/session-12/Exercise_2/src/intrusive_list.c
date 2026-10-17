/**
 * @file    intrusive_list.c
 * @brief   Intrusive singly-linked list implementation.
 */

#include "intrusive_list.h"

#include <inttypes.h>
#include <stdio.h>

/**
 * @brief Push a node at the head in O(1).
 */
void slist_push(slist_node_t **pp_head, slist_node_t *p_node)
{
    if ((pp_head == NULL) || (p_node == NULL))
    {
        return;
    }

    p_node->p_next = *pp_head;
    *pp_head = p_node;
}

/**
 * @brief Pop a node from the head in O(1).
 */
slist_node_t *slist_pop(slist_node_t **pp_head)
{
    slist_node_t *p_node;

    if ((pp_head == NULL) || (*pp_head == NULL))
    {
        return NULL;
    }

    p_node = *pp_head;
    *pp_head = p_node->p_next;
    p_node->p_next = NULL;

    return p_node;
}

/**
 * @brief Append a node at the tail in O(N).
 */
void slist_append(slist_node_t **pp_head, slist_node_t *p_node)
{
    slist_node_t *p_curr;

    if ((pp_head == NULL) || (p_node == NULL))
    {
        return;
    }

    p_node->p_next = NULL;

    if (*pp_head == NULL)
    {
        *pp_head = p_node;
        return;
    }

    p_curr = *pp_head;

    while (p_curr->p_next != NULL)
    {
        p_curr = p_curr->p_next;
    }

    p_curr->p_next = p_node;
}

/**
 * @brief Remove one exact node from the list.
 */
bool slist_remove(slist_node_t **pp_head, slist_node_t *p_target)
{
    slist_node_t *p_prev;
    slist_node_t *p_curr;

    if ((pp_head == NULL) || (*pp_head == NULL) || (p_target == NULL))
    {
        return false;
    }

    p_prev = NULL;
    p_curr = *pp_head;

    while ((p_curr != NULL) && (p_curr != p_target))
    {
        p_prev = p_curr;
        p_curr = p_curr->p_next;
    }

    if (p_curr == NULL)
    {
        return false;
    }

    if (p_prev == NULL)
    {
        *pp_head = p_curr->p_next;
    }
    else
    {
        p_prev->p_next = p_curr->p_next;
    }

    p_curr->p_next = NULL;

    return true;
}

/**
 * @brief Pop all tasks, recover the parent task with CONTAINER_OF,
 *        print task data, and execute callbacks.
 */
void dispatch_deferred_tasks(slist_node_t **pp_head)
{
    slist_node_t *p_node;

    if (pp_head == NULL)
    {
        return;
    }

    p_node = slist_pop(pp_head);

    while (p_node != NULL)
    {
        deferred_task_t *p_task = CONTAINER_OF(p_node, deferred_task_t, node);

        (void)printf(
            "    --> Executing Task ID: %" PRIu32
            " [Pri: %u] | Callback arg: 0x%" PRIX32 "\n",
            p_task->task_id,
            (unsigned int)p_task->priority,
            p_task->arg);

        if (p_task->callback != NULL)
        {
            p_task->callback(p_task->arg);
        }

        p_node = slist_pop(pp_head);
    }
}
