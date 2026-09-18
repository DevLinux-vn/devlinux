/**
 * @file    main.c
 * @brief   Demo of an intrusive deferred task work queue.
 */

#include "intrusive_list.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Battery ADC callback.
 */
static void read_battery_adc(uint32_t arg)
{
    (void)arg;
}

/**
 * @brief BLE packet callback.
 */
static void process_ble_packet(uint32_t arg)
{
    (void)arg;
}

/**
 * @brief Flash-buffer callback.
 */
static void flush_flash_buffer(uint32_t arg)
{
    (void)arg;
}

/**
 * @brief Emergency temperature callback.
 */
static void emergency_temperature_cutoff(uint32_t arg)
{
    (void)arg;
}

/**
 * @brief Program entry point.
 *
 * @return 0 on success, non-zero on error.
 */
int main(void)
{
    slist_node_t *p_head = NULL;

    static deferred_task_t tasks[4] = {
        {
            .task_id = 101U,
            .priority = 1U,
            .callback = read_battery_adc,
            .arg = 0x1234U,
            .node = { .p_next = NULL }
        },
        {
            .task_id = 102U,
            .priority = 3U,
            .callback = process_ble_packet,
            .arg = 0xFACEU,
            .node = { .p_next = NULL }
        },
        {
            .task_id = 103U,
            .priority = 2U,
            .callback = flush_flash_buffer,
            .arg = 0xBEEFU,
            .node = { .p_next = NULL }
        },
        {
            .task_id = 104U,
            .priority = 4U,
            .callback = emergency_temperature_cutoff,
            .arg = 0xDEADU,
            .node = { .p_next = NULL }
        }
    };

    bool removed;

    (void)printf(
        "========== L12 Lab 2: Intrusive List & Deferred Task Queue ==========\n\n");

    (void)printf("[1] Initializing static tasks (zero heap allocation):\n");
    (void)printf("    Task 1: ID=101, Pri=1 (Read Battery ADC)\n");
    (void)printf("    Task 2: ID=102, Pri=3 (Process BLE Packet)\n");
    (void)printf("    Task 3: ID=103, Pri=2 (Flush Flash Buffer)\n");
    (void)printf("    Task 4: ID=104, Pri=4 (Emergency Temperature Cutoff)\n");

    (void)printf("\n[2] Pushing tasks onto Deferred Work Stack...\n");

    slist_push(&p_head, &tasks[0].node);
    slist_push(&p_head, &tasks[1].node);
    slist_push(&p_head, &tasks[2].node);
    slist_push(&p_head, &tasks[3].node);

    (void)printf(
        "[3] Removing Task 103 from list using slist_remove()... ");

    removed = slist_remove(&p_head, &tasks[2].node);

    if (removed)
    {
        (void)printf("OK\n");
    }
    else
    {
        (void)printf("FAILED\n");
        return 1;
    }

    (void)printf(
        "\n[4] Dispatching remaining tasks (LIFO execution order):\n");

    dispatch_deferred_tasks(&p_head);

    if (p_head != NULL)
    {
        (void)printf("\nERROR: queue is not empty after dispatch.\n");
        return 1;
    }

    (void)printf(
        "\nAll deferred tasks dispatched successfully. Queue is empty.\n");

    return 0;
}
