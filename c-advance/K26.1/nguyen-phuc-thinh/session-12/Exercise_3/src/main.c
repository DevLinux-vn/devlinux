#include "ring_buffer_lf.h"
#include <stdio.h>

int main(void)
{
    ring_buf_lf_t rb;
    rb_init(&rb);

    printf("========== L12 Lab 3: SPSC Lock-Free Ring Buffer ==========\n\n");
    
    printf("[!] Initializing Ring Buffer (Capacity: %u, Max Usable: %u)...\n", RB_CAPACITY, RB_CAPACITY - 1U);
    printf("    State: Empty=%s, Full=%s, Available=%u\n\n",
           rb_is_empty(&rb) ? "TRUE" : "FALSE",
           rb_is_full(&rb) ? "TRUE" : "FALSE",
           rb_available(&rb));

    printf("[!] Pushing 7 items until buffer is full:\n");
    for (int16_t i = 10; i <= 70; i += 10)
    {
        if (rb_push(&rb, i))
        {
            printf("    Push %d -> OK (Available: %u)", i, rb_available(&rb));
            if (rb_is_full(&rb))
            {
                printf(" [FULL]");
            }
            printf("\n");
        }
    }

    printf("\n[!] Attempting 8th push (data=80) into full buffer:\n");
    if (!rb_push(&rb, 80))
    {
        printf("    Push 80 -> REJECTED (Buffer Full, Waste-One-slot preserved)\n\n");
    }

    printf("[!] Popping 3 items:\n");
    int16_t val;
    for (int i = 0; i < 3; i++)
    {
        if (rb_pop(&rb, &val))
        {
            printf("    Popped: %d (Available: %u)\n", val, rb_available(&rb));
        }
    }

    printf("\n[!] Pushing 3 more items (forcing head pointer wrap-around):\n");
    int16_t more_items[] = {80, 90, 100};
    for (int i = 0; i < 3; i++)
    {
        if (rb_push(&rb, more_items[i]))
        {
            printf("    Push %d -> OK (Available: %u)\n", more_items[i], rb_available(&rb));
        }
    }

    printf("\n[!] Popping all remaining items:\n    Popped: ");
    bool first = true;
    while (rb_pop(&rb, &val))
    {
        if (!first)
        {
            printf(", ");
        }
        printf("%d", val);
        first = false;
    }
    printf("\n    Buffer is now empty.\n");

    return 0;
}