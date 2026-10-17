#include "sorted_list.h"
#include <stdio.h>

int main(void)
{
    event_node_t *p_head = NULL;
    uint32_t max_capacity = 5;

    printf("========== L12 Lab 1: Sorted Sensor Event List ==========\n\n");
    
    printf("[1] Inserting 6 events (out-of-order) with max_capacity = 5:\n");
    sorted_insert(&p_head, create_event_node(100, 2, 250), max_capacity);
    sorted_insert(&p_head, create_event_node(50, 1, 240), max_capacity);
    sorted_insert(&p_head, create_event_node(200, 3, 260), max_capacity);
    sorted_insert(&p_head, create_event_node(25, 4, 210), max_capacity);
    sorted_insert(&p_head, create_event_node(150, 5, 220), max_capacity);
    sorted_insert(&p_head, create_event_node(300, 6, 280), max_capacity);

    printf("\n--- Current Sorted Event List (Length: %u) ---\n", get_list_length(p_head));
    print_events(p_head);

    printf("\n[2] Deleting event with Sensor ID = 5 (middle node)...\n");
    if (delete_event_by_id(&p_head, 5))
    {
        printf("    Deleted successfully.\n");
    }

    printf("\n--- Event List After Deletion (Length: %u) ---\n", get_list_length(p_head));
    print_events(p_head);

    printf("\n[3] Freeing entire list... ");
    free_list(&p_head);
    printf("done.\n");

    return 0;
}