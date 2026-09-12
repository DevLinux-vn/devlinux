#include "intrusive_list.h"
#include <stdint.h>
#include <stdio.h>

#define APP_SUCCESS     (0)

int32_t main() {
    printf("========== L12 Lab 2: Intrusive List & Deferred Task Queue ==========\n");

    printf("\n[1] Initializing static tasks (zero heap allocation):\n");
    deferred_task_t tasks[4] = {
        {
            .task_id = 101U,
            .priority = 1U,
            .callback = NULL,
            .arg = 0x1234
        },
        {
            .task_id = 102U,
            .priority = 3U,
            .callback = NULL,
            .arg = 0xFACE
        },
        {
            .task_id = 103U,
            .priority = 2U,
            .callback = NULL,
            .arg = 0x1324
        },
        {
            .task_id = 104U,
            .priority = 4U,
            .callback = NULL,
            .arg = 0xDEAD
        }
    };

    slist_node_t *p_head = NULL;

    printf("Task 1: ID=%u, Pri=%u (Read Battery ADC)\n", tasks[0].task_id, tasks[0].priority);
    printf("Task 2: ID=%u, Pri=%u (Process BLE Packet)\n", tasks[1].task_id, tasks[1].priority);
    printf("Task 3: ID=%u, Pri=%u (Flush Flash Buffer)\n", tasks[2].task_id, tasks[2].priority);
    printf("Task 4: ID=%u, Pri=%u (Emergency Temperature Cutoff)\n", tasks[3].task_id, tasks[3].priority);

    printf("\n[2] Pushing tasks onto Deferred Work Stack...\n");
    for (size_t i = 0U; i < 4U; i++) {
        slist_push(&p_head, &tasks[i].node);
    }

    printf("[3] Removing Task 103 from list using slist_remove()... %s\n", slist_remove(&p_head, &tasks[2].node) ? "OK" : "FAILED");

    printf("\n[4] Dispatching remaining tasks (LIFO execution order):\n");
    dispatch_deferred_tasks(&p_head);

    printf("\nAll deferred tasks dispatched successfully. Queue is empty.\n");

    return APP_SUCCESS;
}