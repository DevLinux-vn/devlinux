#ifndef INTRUSIVE_LIST_H
#define INTRUSIVE_LIST_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Retrieve the address of the containing structure from a member pointer.
 *
 * @param ptr    Pointer to the structure member.
 * @param type   Type of the containing structure.
 * @param member Name of the member within the containing structure.
 *
 * @return Pointer to the containing structure.
 */
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/**
 * @brief Node structure for an intrusive singly-linked list.
 *
 * This node is embedded directly inside the payload structure.
 */
typedef struct slist_node {
    struct slist_node *p_next;
} slist_node_t;

/**
 * @brief Function pointer type for a deferred task callback.
 *
 * @param arg Argument passed to the callback function.
 */
typedef void (*task_callback_t)(uint32_t arg);

/**
 * @brief Structure representing a deferred task.
 *
 * Contains task information and an embedded intrusive list node.
 */
typedef struct {
    uint32_t        task_id;
    uint8_t         priority;
    task_callback_t callback;
    uint32_t        arg;
    slist_node_t    node;
} deferred_task_t;

/**
 * @brief Push a node to the head of the singly-linked list.
 *
 * This operation follows LIFO behavior and has O(1) time complexity.
 *
 * @param pp_head Address of the list head pointer.
 * @param p_node  Pointer to the node to be inserted.
 */
void slist_push(slist_node_t **pp_head, slist_node_t *p_node);

/**
 * @brief Pop a node from the head of the singly-linked list.
 *
 * This operation follows LIFO behavior and has O(1) time complexity.
 *
 * @param pp_head Address of the list head pointer.
 *
 * @return Pointer to the removed node, or NULL if the list is empty.
 */
slist_node_t *slist_pop(slist_node_t **pp_head);

/**
 * @brief Append a node to the end of the singly-linked list.
 *
 * This operation follows FIFO behavior and has O(N) time complexity.
 *
 * @param pp_head Address of the list head pointer.
 * @param p_node  Pointer to the node to be appended.
 */
void slist_append(slist_node_t **pp_head, slist_node_t *p_node);

/**
 * @brief Remove a specific node from the singly-linked list.
 *
 * @param pp_head  Address of the list head pointer.
 * @param p_target Pointer to the node to be removed.
 *
 * @return true if the node was found and removed, otherwise false.
 */
bool slist_remove(slist_node_t **pp_head, const slist_node_t *p_target);

/**
 * @brief Dispatch all deferred tasks in the list.
 *
 * Pops each node from the list, retrieves the containing deferred_task_t
 * using CONTAINER_OF(), and executes its callback.
 *
 * @param pp_head Address of the list head pointer.
 */
void dispatch_deferred_tasks(slist_node_t **pp_head);

#endif /* INTRUSIVE_LIST_H */