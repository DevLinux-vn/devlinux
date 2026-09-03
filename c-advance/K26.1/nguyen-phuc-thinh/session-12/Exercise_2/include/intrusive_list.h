/**
 * @file intrusive_list.h
 * @brief Intrusive linked list definitions and operations.
 */
#ifndef INTRUSIVE_LIST_H
#define INTRUSIVE_LIST_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Macro to retrieve the parent structure pointer from a member pointer.
 * 
 * Complies with MISRA-C Rule 11.4 and CERT-C EXP36-C by correctly casting
 * to char* for byte-level pointer arithmetic before casting back.
 */
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/**
 * @brief Embedded intrusive list node.
 */
typedef struct slist_node
{
    struct slist_node *p_next;
} slist_node_t;

/**
 * @brief Function pointer type for deferred task callbacks.
 */
typedef void (*task_callback_t)(uint32_t arg);

/**
 * @brief Structure representing a deferred task in a queue.
 */
typedef struct
{
    uint32_t        task_id;
    uint8_t         priority;
    task_callback_t callback;
    uint32_t        arg;
    slist_node_t    node; /* Embedded intrusive list node */
} deferred_task_t;

/**
 * @brief Pushes a node to the front of the list (LIFO).
 * @param pp_head Double pointer to the list head.
 * @param p_node Pointer to the node to insert.
 */
void slist_push(slist_node_t **pp_head, slist_node_t *p_node);

/**
 * @brief Pops a node from the front of the list.
 * @param pp_head Double pointer to the list head.
 * @return Pointer to the popped node, or NULL if empty.
 */
slist_node_t* slist_pop(slist_node_t **pp_head);

/**
 * @brief Appends a node to the end of the list (FIFO).
 * @param pp_head Double pointer to the list head.
 * @param p_node Pointer to the node to append.
 */
void slist_append(slist_node_t **pp_head, slist_node_t *p_node);

/**
 * @brief Removes a specific node from the list.
 * @param pp_head Double pointer to the list head.
 * @param p_target Pointer to the exact node to remove.
 * @return true if found and removed, false otherwise.
 */
bool slist_remove(slist_node_t **pp_head, slist_node_t *p_target);

/**
 * @brief Pops all tasks from the list, recovers the parent struct, and executes them.
 * @param pp_head Double pointer to the list head.
 */
void dispatch_deferred_tasks(slist_node_t **pp_head);

#endif /* INTRUSIVE_LIST_H */