#include "ring_buffer.h"
#include <stdint.h>
#include <stdio.h>

#define APP_SUCCESS (0U)

int32_t main() {
  uint8_t temp = 0U;

  ring_buffer_t rb;
  ring_init(&rb);

  printf("=== Test 1: Normal push/pop operations ===\n");

  ring_push(&rb, 10U);
  printf("Pushed: 10 (count = %u)\n", ring_count(&rb));

  ring_push(&rb, 20U);
  printf("Pushed: 20 (count = %u)\n", ring_count(&rb));

  temp = ring_pop(&rb);
  printf("Popped: %u (count = %u)\n", temp, ring_count(&rb));

  temp = ring_pop(&rb);
  printf("Popped: %u (count = %u)\n", temp, ring_count(&rb));

  printf("=== Test 2: Contract Violation Test ===\n");

  for (size_t i = 0U; i < RING_CAPACITY; i++) {
    ring_push(&rb, (uint8_t)i);
  }

  printf("Buffer is full. Pushing one more item...\n");
  ring_push(&rb, 100U);

  return APP_SUCCESS;
}