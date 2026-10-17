#include "intrusive_list.h"
#include <stdio.h>

static void dummy_callback(uint32_t arg)
{
    (void)arg; /* Prevent unused parameter warning */
}

int main(void)
{
    slist_node_t *p_queue_head = NULL;

    printf("========== L12 Lab 2: Intrusive List & Deferred Task Queue ==========\n\n");
    printf("[1] Initializing static tasks (zero heap allocation):\n");
    printf("    Task 1: ID=101, Pri=1 (Read Battery ADC)\n");
    printf("    Task 2: ID=102, Pri=3 (Process BLE Packet)\n");
    printf("    Task 3: ID=103, Pri=2 (Flush Flash Buffer)\n");
    printf("    Task 4: ID=104, Pri=4 (Emergency Temperature Cutoff)\n\n");

    /* Static allocation per requirements - NO MALLOC */
    deferred_task_t tasks[4] = {
        {101, 1, dummy_callback, 0x1234, {NULL}},
        {102, 3, dummy_callback, 0xFACE, {NULL}},
        {103, 2, dummy_callback, 0xBEEF, {NULL}},
        {104, 4, dummy_callback, 0xDEAD, {NULL}}
    };

    printf("[2] Pushing tasks onto Deferred Work Stack...\n");
    slist_push(&p_queue_head, &tasks[0].node);
    slist_push(&p_queue_head, &tasks[1].node);
    slist_push(&p_queue_head, &tasks[2].node);
    slist_push(&p_queue_head, &tasks[3].node);

    printf("[3] Removing Task 103 from list using slist_remove()... ");
    if (slist_remove(&p_queue_head, &tasks[2].node))
    {
        printf("OK\n\n");
    }
    else
    {
        printf("FAILED\n\n");
    }

    printf("[4] Dispatching remaining tasks (LIFO execution order):\n");
    dispatch_deferred_tasks(&p_queue_head);

    printf("\nAll deferred tasks dispatched successfully. Queue is empty.\n");

    return 0;
}