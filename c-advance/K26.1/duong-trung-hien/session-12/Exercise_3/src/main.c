#include "ring_buffer_lf.h"
#include <stdio.h>
#include <stdint.h>

#define APP_SUCCESS             (0)
#define SIZE_POP                (3U)
#define BOOL_TO_STRING(x)       ((x) ? "TRUE" : "FALSE")
#define BOOL_TO_STATUS(x)       ((x) ? "OK" : "FAIL")
#define RB_STATUS_FULL(is_full) ((is_full) ? "FULL" : "NOT FULL")

int32_t main() {
    printf("========== L12 Lab 3: SPSC Lock-Free Ring Buffer ==========\n");

    printf("\n[!] Initializing Ring Buffer (Capacity: %u, Max Usable: %u)...\n", RB_CAPACITY, RB_CAPACITY - 1U);

    ring_buf_lf_t data;
    rb_init(&data);

    printf("State: Empty=%s, Full=%s, Available=%u\n", BOOL_TO_STRING(rb_is_empty(&data)),
                                                     BOOL_TO_STRING(rb_is_full(&data)),
                                                     rb_available(&data) );
    
    printf("\n[!] Pushing %u items until buffer is full:\n", RB_CAPACITY - 1U);

    for (size_t i = 0U; i < (RB_CAPACITY - 1); i++) {
        bool result = rb_push(&data, (i + 1U) * 10U);

        printf("Push %d -> %s (Available: %u)\n", data.buffer[i], BOOL_TO_STATUS(result), rb_available(&data));

        if (rb_is_full(&data)) {
            printf("%s\n", RB_STATUS_FULL(rb_is_full(&data)));
        }
    }

    printf("\n[!] Attempting 8th push (data=80) into full buffer:\n");
    if (!rb_push(&data, 80U)) {
        printf("Push 80 -> REJECTED (Buffer Full, Waste-One-Slot preserved)\n");
    }

    printf("\n[!] Popping 3 items:\n");
    int16_t restored[RB_CAPACITY - 1U];

    for (size_t i = 0U; i < SIZE_POP; i++) {
        rb_pop(&data, &restored[i]);

        printf("Popped: %d (Available: %u)\n", data.buffer[i], rb_available(&data));
    }

    printf("\n[!] Pushing 3 more items (forcing head pointer wrap-around):\n");
    
    for (size_t i = 0U; i < SIZE_POP; i++) {
        int16_t value = (i * 10U) + 80U;
        bool result = rb_push(&data, value);

        printf("Push %d -> %s (Available: %u)\n", value, BOOL_TO_STATUS(result), rb_available(&data));
    }

    printf("\n[!] Popping all remaining items:\n");
    printf("Popped: ");

    for (size_t i = 0U; i < (RB_CAPACITY - 1U); i++) {
        rb_pop(&data, &restored[i]);
        
        printf("%d ", restored[i]);
    }
    
    printf("\nBuffer is now empty.\n");

    return APP_SUCCESS;
}