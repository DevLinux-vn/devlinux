#include "intrusive_list.h"
#include <stdio.h>

void slist_push(slist_node_t **pp_head, slist_node_t *p_node)
{
    if ((pp_head != NULL) && (p_node != NULL))
    {
        p_node->p_next = *pp_head;
        *pp_head = p_node;
    }
}

slist_node_t* slist_pop(slist_node_t **pp_head)
{
    if ((pp_head == NULL) || (*pp_head == NULL))
    {
        return NULL;
    }
    
    slist_node_t *p_popped = *pp_head;
    *pp_head = p_popped->p_next;
    p_popped->p_next = NULL;
    return p_popped;
}

void slist_append(slist_node_t **pp_head, slist_node_t *p_node)
{
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

    slist_node_t *p_curr = *pp_head;
    while (p_curr->p_next != NULL)
    {
        p_curr = p_curr->p_next;
    }
    p_curr->p_next = p_node;
}

bool slist_remove(slist_node_t **pp_head, slist_node_t *p_target)
{
    if ((pp_head == NULL) || (*pp_head == NULL) || (p_target == NULL))
    {
        return false;
    }

    if (*pp_head == p_target)
    {
        *pp_head = p_target->p_next;
        p_target->p_next = NULL;
        return true;
    }

    slist_node_t *p_curr = *pp_head;
    while (p_curr->p_next != NULL)
    {
        if (p_curr->p_next == p_target)
        {
            p_curr->p_next = p_target->p_next;
            p_target->p_next = NULL;
            return true;
        }
        p_curr = p_curr->p_next;
    }

    return false;
}

void dispatch_deferred_tasks(slist_node_t **pp_head)
{
    slist_node_t *p_curr_node = slist_pop(pp_head);
    
    while (p_curr_node != NULL)
    {
        deferred_task_t *p_task = CONTAINER_OF(p_curr_node, deferred_task_t, node);
        
        printf("  --> Executing Task ID: %u [Pri: %u] | Callback arg: 0x%X\n",
               p_task->task_id, p_task->priority, p_task->arg);
               
        if (p_task->callback != NULL)
        {
            p_task->callback(p_task->arg);
        }
        
        p_curr_node = slist_pop(pp_head);
    }
}