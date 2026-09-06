#include "ring_buffer.h"
#include <stdio.h>

int main(void)
{
    ring_buffer_t rb;
    ring_init(&rb);

    printf("=== Test 1: Normal push/pop operations ===\n");
    ring_push(&rb, 10);
    printf("Pushed: 10 (count = %u)\n", ring_count(&rb));
    ring_push(&rb, 20);
    printf("Pushed: 20 (count = %u)\n", ring_count(&rb));
    
    printf("Popped: %u (count = %u)\n", ring_pop(&rb), ring_count(&rb));
    printf("Popped: %u (count = %u)\n", ring_pop(&rb), ring_count(&rb));

    printf("=== Test 2: Contract Violation Test ===\n");
    printf("Buffer is full. Pushing one more item...\n");
    
    for (uint32_t i = 0; i < RING_CAPACITY; i++)
    {
        ring_push(&rb, (uint8_t)i);
    }
    
    /* Gây lỗi tràn bộ đệm có chủ ý để kích hoạt REQUIRE */
    ring_push(&rb, 99);

    return 0;
}