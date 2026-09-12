#include "sorted_list.h"
#include <stdio.h>
#include <stdint.h>

#define APP_SUCCESS         (0)
#define MAX_CAPACITY        (5U)
#define EVENT_NODE_COUNT    (6U) 

typedef struct {
    uint32_t timestamp;
    uint8_t  sensor_id;
    int16_t  reading;
} event_data_t;

int32_t main() {
    printf("========== L12 Lab 1: Sorted Sensor Event List ==========\n\n");

    event_node_t *p_head = NULL;
    event_node_t *p_node = NULL;

    event_data_t events[] =
    {
        {100U, 2U, 250},
        {50U,  1U, 240},
        {200U, 3U, 260},
        {25U,  4U, 210},
        {150U, 5U, 220},
        {300U, 6U, 280}
    };

    printf("[1] Inserting %u events (out-of-order) with max_capacity = %u:\n", EVENT_NODE_COUNT, MAX_CAPACITY);

    for (uint32_t i = 0U; i < EVENT_NODE_COUNT; i++) {
        p_node = create_event_node(events[i].timestamp, events[i].sensor_id, events[i].reading);
        sorted_insert(&p_head, p_node, MAX_CAPACITY);
        printf("Inserted: [t=%-3u, ID=%u, Val=%-3d]\n", events[i].timestamp, events[i].sensor_id, events[i].reading);

        if (i == MAX_CAPACITY) {
            printf("-> Capacity exceeded! Evicted oldest [t=%u].\n\n", p_head->timestamp);
        }

    }

    uint32_t count_list = get_list_length(p_head);
    printf("--- Current Sorted Event List (Length: %u) ---\n", count_list);
    print_events(p_head);

    printf("\n[2] Deleting event with Sensor ID = 5 (middle node)...\n");

    if (delete_event_by_id(&p_head, 5U)) {
        printf("Deleted successfully.\n");
    }
    else {
        printf("Deleted failed.\n");
    }

    count_list = get_list_length(p_head);
    printf("\n--- Event List After Deletion (Length: %u) ---\n", count_list);
    print_events(p_head);

    printf("\n[3] Freeing entire list... done.\n");
    free_list(&p_head);

    return APP_SUCCESS;
}