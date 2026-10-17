/**
 * @file    main.c
 * @brief   Demonstration of the bounded, sorted sensor event list:
 *          out-of-order insertion with automatic eviction, deletion
 *          by sensor ID, and full cleanup.
 */
#include "sorted_list.h"

#include <stdio.h>

/** @brief Maximum number of events the demo list may hold at once. */
#define DEMO_MAX_CAPACITY (5U)

/** @brief One raw input event used to seed the demo list. */
typedef struct
{
    uint32_t timestamp;
    uint8_t sensor_id;
    int16_t reading;
} demo_event_t;

/**
 * @brief Program entry point. Runs the insert/evict/delete/free demo
 *        described in the exercise.
 *
 * @return 0 on success.
 */
int main(void)
{
    /* Six events, deliberately out of timestamp order, to exercise
     * head/middle/tail insertion and (on the 6th insert) eviction. */
    static const demo_event_t demo_events[] = {
        { 100U, 2U, 250 },
        { 50U, 1U, 240 },
        { 200U, 3U, 260 },
        { 25U, 4U, 210 },
        { 150U, 5U, 220 },
        { 300U, 6U, 280 },
    };
    static const size_t event_count = sizeof(demo_events) / sizeof(demo_events[0]);

    event_node_t *p_head = NULL;
    size_t i;

    (void)printf("========== L12 Lab 1: Sorted Sensor Event List ==========\n\n");
    (void)printf("[1] Inserting %zu events (out-of-order) with max_capacity = %u:\n",
                 event_count, DEMO_MAX_CAPACITY);

    for (i = 0U; i < event_count; i++)
    {
        event_node_t *p_node = create_event_node(demo_events[i].timestamp,
                                                   demo_events[i].sensor_id,
                                                   demo_events[i].reading);

        if (p_node == NULL)
        {
            (void)printf("    ERROR: allocation failed for event t=%u\n",
                         demo_events[i].timestamp);
            continue;
        }

        /* Capture whether the list is already at capacity BEFORE
         * this insertion, and note the current oldest timestamp: if
         * the list is full, this insertion will trigger an eviction
         * of exactly this node. */
        const uint32_t length_before = get_list_length(p_head);
        const bool will_evict = (length_before == DEMO_MAX_CAPACITY);
        /* p_head cannot actually be NULL when will_evict is true
         * (get_list_length(NULL) is 0, which never equals
         * DEMO_MAX_CAPACITY), but the extra check keeps this
         * obviously safe to a static analyzer without relying on
         * that cross-function reasoning. */
        const uint32_t oldest_before = ((will_evict) && (p_head != NULL)) ? p_head->timestamp : 0U;

        (void)sorted_insert(&p_head, p_node, DEMO_MAX_CAPACITY);

        (void)printf("    Inserted: [t=%u, ID=%u, Val=%d]",
                     demo_events[i].timestamp,
                     demo_events[i].sensor_id,
                     demo_events[i].reading);

        if (will_evict)
        {
            (void)printf(" -> Capacity exceeded! Evicted oldest [t=%u].", oldest_before);
        }

        (void)printf("\n");
    }

    (void)printf("\n--- Current Sorted Event List (Length: %u) ---\n", get_list_length(p_head));
    print_events(p_head);

    (void)printf("\n[2] Deleting event with Sensor ID = 5 (middle node)...\n");
    if (delete_event_by_id(&p_head, 5U))
    {
        (void)printf("    Deleted successfully.\n");
    }
    else
    {
        (void)printf("    Sensor ID not found.\n");
    }

    (void)printf("\n--- Event List After Deletion (Length: %u) ---\n", get_list_length(p_head));
    print_events(p_head);

    (void)printf("\n[3] Freeing entire list... ");
    free_list(&p_head);
    (void)printf("done.\n");

    return 0;
}