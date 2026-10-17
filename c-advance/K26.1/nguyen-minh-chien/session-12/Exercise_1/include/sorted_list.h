/**
 * @file    sorted_list.h
 * @brief   Bounded-capacity, timestamp-sorted singly-linked list of
 *          sensor events (traditional / non-intrusive linked list).
 *
 * @details Each event_node_t couples its payload (timestamp,
 *          sensor_id, reading) together with the list linkage
 *          (p_next) in a single struct. This is simple to use but
 *          means the list logic (sorted_insert, delete_event_by_id,
 *          ...) can only ever operate on event_node_t -- it cannot
 *          be reused for any other payload type without being
 *          rewritten. Contrast this with an "intrusive" linked list,
 *          where the linkage lives in a small, payload-agnostic node
 *          embedded inside arbitrary structs, letting the same list
 *          code manage any data type.
 */
#ifndef SORTED_LIST_H
#define SORTED_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A single sensor event, and the singly-linked-list node that
 *        carries it.
 */
typedef struct event_node
{
    uint32_t timestamp;        /**< Event timestamp (sort key, ascending). */
    uint8_t sensor_id;         /**< ID of the sensor that produced this event. */
    int16_t reading;           /**< Sensor reading value. */
    struct event_node *p_next; /**< Next node in the list, or NULL at the tail. */
} event_node_t;

/**
 * @brief Allocate and initialize a new, unlinked event node.
 *
 * @param[in] timestamp Event timestamp.
 * @param[in] sensor_id ID of the sensor that produced the event.
 * @param[in] reading   Sensor reading value.
 *
 * @return Pointer to the newly allocated node (its @c p_next is
 *         NULL), or NULL if allocation failed.
 *
 * @note Ownership: the caller owns the returned node until it is
 *       either passed to sorted_insert() (which takes ownership on
 *       success) or freed directly by the caller.
 */
event_node_t *create_event_node(uint32_t timestamp, uint8_t sensor_id, int16_t reading);

/**
 * @brief Insert a node into the list in ascending-timestamp order,
 *        evicting the oldest node if this would exceed capacity.
 *
 * @param[in,out] pp_head       Address of the caller's list head
 *                                pointer. Must not be NULL. May be
 *                                updated if @p p_new becomes the new
 *                                head, or if the old head is evicted.
 * @param[in]     p_new         The node to insert (typically from
 *                                create_event_node()). Must not be
 *                                NULL.
 * @param[in]     max_capacity  Maximum number of nodes the list may
 *                                hold after this insertion. If
 *                                inserting @p p_new would make the
 *                                list longer than this, the node
 *                                with the smallest timestamp (the
 *                                current head) is removed and freed.
 *
 * @return true on success (the node was linked into the list,
 *         possibly triggering an eviction).
 * @return false if @p pp_head or @p p_new is NULL; in that case
 *         @p p_new is NOT freed -- the caller still owns it.
 *
 * @note Ownership: on success, the list owns @p p_new (and will
 *       free() it later via delete_event_by_id() or free_list()).
 *       If @p p_new's own timestamp is the smallest in an
 *       already-full list, it is possible for @p p_new itself to be
 *       the node evicted immediately after insertion -- this is
 *       correct "oldest wins eviction" behavior, not a bug.
 */
bool sorted_insert(event_node_t **pp_head, event_node_t *p_new, uint32_t max_capacity);

/**
 * @brief Find the first node matching @p sensor_id, unlink it, and
 *        free() it.
 *
 * @param[in,out] pp_head   Address of the caller's list head
 *                           pointer. Must not be NULL.
 * @param[in]     sensor_id The sensor ID to search for.
 *
 * @return true  if a matching node was found, unlinked, and freed.
 * @return false if @p pp_head is NULL, the list is empty, or no
 *               node matches @p sensor_id (list is left unchanged).
 */
bool delete_event_by_id(event_node_t **pp_head, uint8_t sensor_id);

/**
 * @brief Count the number of nodes currently in the list.
 *
 * @param[in] p_head The list head (may be NULL for an empty list).
 * @return Number of nodes in the list (0 if @p p_head is NULL).
 */
uint32_t get_list_length(const event_node_t *p_head);

/**
 * @brief Print every event in the list, head to tail.
 *
 * @param[in] p_head The list head (may be NULL for an empty list,
 *                    in which case nothing is printed).
 */
void print_events(const event_node_t *p_head);

/**
 * @brief Free every node in the list and reset the head to NULL.
 *
 * @param[in,out] pp_head Address of the caller's list head pointer.
 *                         If NULL, this function does nothing. If
 *                         @c *pp_head is already NULL, this function
 *                         does nothing (the list is already empty).
 *
 * @details Saves each node's @c p_next before calling free() on it,
 *          to avoid dereferencing freed memory (use-after-free).
 *          Always sets @c *pp_head to NULL on completion, so the
 *          caller's pointer can never be left dangling.
 */
void free_list(event_node_t **pp_head);

#ifdef __cplusplus
}
#endif

#endif /* SORTED_LIST_H */