#include "intrusive_list.h"
#include <stdio.h>

void slist_push(slist_node_t **pp_head, slist_node_t *p_node) {
    if (NULL == p_node || NULL == pp_head) {
        return;
    }

    p_node->p_next = *pp_head;
    *pp_head = p_node;
}

slist_node_t* slist_pop(slist_node_t **pp_head) {
    if (NULL == pp_head || NULL == *pp_head) {
        return NULL;
    }

    slist_node_t *p_current = *pp_head;
    *pp_head = p_current->p_next;
    
    p_current->p_next = NULL;

    return p_current;
}

void slist_append(slist_node_t **pp_head, slist_node_t *p_node) {
    if (NULL == p_node || NULL == pp_head) {
        return;
    }

    if (NULL == *pp_head) {
        *pp_head = p_node;
        p_node->p_next = NULL;

        return;
    }

    slist_node_t *p_current = *pp_head;

    while (NULL != p_current->p_next) {
        p_current = p_current->p_next;
    }

    p_current->p_next = p_node;
    p_node->p_next = NULL;
}

bool slist_remove(slist_node_t **pp_head, const slist_node_t *p_target) {
    if (NULL == p_target || NULL == pp_head || NULL == *pp_head) {
        return false;
    }

    slist_node_t *p_current = *pp_head;
    slist_node_t *p_previous = NULL;

    while (NULL != p_current && p_current != p_target) {
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

    return true;
}

void dispatch_deferred_tasks(slist_node_t **pp_head) {
    if (NULL == pp_head) {
        return;
    }

    slist_node_t *p_node = NULL;

    while (NULL != (p_node = slist_pop(pp_head))) {
        deferred_task_t *p_task = CONTAINER_OF(p_node, deferred_task_t, node);
        printf("--> Executing Task ID: %u [Pri: %u] | Callback arg: 0x%4X\n", p_task->task_id, p_task->priority, p_task->arg);

        if (NULL != p_task->callback) {
            p_task->callback(p_task->arg);
        }
    }
}