/**
 * @file main.c
 * @brief Demonstration program for the SPSC lock-free ring buffer.
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "ring_buffer_lf.h"

int main(void)
{
    ring_buf_lf_t rb;
    int16_t data;
    bool status;
    const int16_t initial_values[7] = {10, 20, 30, 40, 50, 60, 70};
    const int16_t wrap_values[3] = {80, 90, 100};
    uint32_t index;

    (void)printf("========== L12 Lab 3: SPSC Lock-Free Ring Buffer ==========\n\n");

    rb_init(&rb);

    (void)printf("[!] Initializing Ring Buffer (Capacity: %u, Max Usable: %u)...\n",
                 (unsigned int)RB_CAPACITY,
                 (unsigned int)(RB_CAPACITY - 1U));
    (void)printf("    State: Empty=%s, Full=%s, Available=%" PRIu32 "\n\n",
                 rb_is_empty(&rb) ? "TRUE" : "FALSE",
                 rb_is_full(&rb) ? "TRUE" : "FALSE",
                 rb_available(&rb));

    (void)printf("[!] Pushing 7 items until buffer is full:\n");

    for (index = 0U; index < 7U; ++index)
    {
        status = rb_push(&rb, initial_values[index]);

        (void)printf("    Push %d -> %s (Available: %" PRIu32 ")%s\n",
                     (int)initial_values[index],
                     status ? "OK" : "FAILED",
                     rb_available(&rb),
                     rb_is_full(&rb) ? " [FULL]" : "");
    }

    (void)printf("\n[!] Attempting 8th push (data=80) into full buffer:\n");
    status = rb_push(&rb, 80);
    (void)printf("    Push 80 -> %s\n",
                 status ? "OK" : "REJECTED (Buffer Full, Waste-One-Slot preserved)");

    if (status)
    {
        return 1;
    }

    (void)printf("\n[!] Popping 3 items:\n");

    for (index = 0U; index < 3U; ++index)
    {
        if (!rb_pop(&rb, &data))
        {
            return 1;
        }

        (void)printf("    Popped: %d (Available: %" PRIu32 ")\n",
                     (int)data,
                     rb_available(&rb));
    }

    (void)printf("\n[!] Pushing 3 more items (forcing head pointer wrap-around):\n");

    for (index = 0U; index < 3U; ++index)
    {
        status = rb_push(&rb, wrap_values[index]);

        if (!status)
        {
            return 1;
        }

        (void)printf("    Push %d -> OK (Available: %" PRIu32 ")\n",
                     (int)wrap_values[index],
                     rb_available(&rb));
    }

    (void)printf("\n[!] Popping all remaining items:\n    Popped: ");

    while (rb_pop(&rb, &data))
    {
        (void)printf("%d", (int)data);

        if (!rb_is_empty(&rb))
        {
            (void)printf(", ");
        }
    }

    (void)printf("\n");

    if (!rb_is_empty(&rb))
    {
        return 1;
    }

    (void)printf("    Buffer is now empty.\n");
    return 0;
}
