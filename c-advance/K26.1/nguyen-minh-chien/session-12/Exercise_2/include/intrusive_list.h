/**
 * @file    intrusive_list.h
 * @brief   Intrusive singly-linked list and deferred task queue interface.
 */

#ifndef INTRUSIVE_LIST_H
#define INTRUSIVE_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Recover the parent structure address from an embedded member address.
 */
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/**
 * @brief Generic intrusive singly-linked-list node.
 */
typedef struct slist_node
{
    struct slist_node *p_next;
} slist_node_t;

/**
 * @brief Deferred-task callback type.
 *
 * @param arg User callback argument.
 */
typedef void (*task_callback_t)(uint32_t arg);

/**
 * @brief Application payload for one deferred task.
 */
typedef struct
{
    uint32_t task_id;
    uint8_t priority;
    task_callback_t callback;
    uint32_t arg;
    slist_node_t node;
} deferred_task_t;

/**
 * @brief Push a node at the list head.
 *
 * @param pp_head Address of the caller's head pointer.
 * @param p_node Node to push.
 */
void slist_push(slist_node_t **pp_head, slist_node_t *p_node);

/**
 * @brief Pop and detach the head node.
 *
 * @param pp_head Address of the caller's head pointer.
 * @return Removed node, or NULL if the list is empty.
 */
slist_node_t *slist_pop(slist_node_t **pp_head);

/**
 * @brief Append a node at the list tail.
 *
 * @param pp_head Address of the caller's head pointer.
 * @param p_node Node to append.
 */
void slist_append(slist_node_t **pp_head, slist_node_t *p_node);

/**
 * @brief Remove a specific node from the list.
 *
 * @param pp_head Address of the caller's head pointer.
 * @param p_target Exact node to remove.
 * @return true if removed, otherwise false.
 */
bool slist_remove(slist_node_t **pp_head, slist_node_t *p_target);

/**
 * @brief Dispatch all deferred tasks in LIFO order.
 *
 * @param pp_head Address of the deferred queue head pointer.
 */
void dispatch_deferred_tasks(slist_node_t **pp_head);

#endif /* INTRUSIVE_LIST_H */
