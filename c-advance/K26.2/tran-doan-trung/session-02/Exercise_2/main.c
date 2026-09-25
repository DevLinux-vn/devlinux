#include <stdint.h>
#include <stdio.h>

/**
 * @brief Monitor stack depth during recursion and abort if limit is exceeded.
 *
 * @param[in] current_depth     Current recursion depth (starts at 0).
 * @param[in] max_depth         Maximum recursion depth to attempt.
 * @param[in] stack_base_addr   Address of a local variable in main(), cast to uintptr_t.
 * @param[in] stack_limit_bytes Maximum allowed stack consumption in bytes.
 * @return 0 on success (max_depth reached), -1 if stack limit was exceeded.
 */

#define STACK_LIMIT_BYTES_DEFAULT (4096U)
#define MAX_RECURSION_DEPTH       (100U)

static uint32_t s_deepest_depth = 0U;

int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth,
                             const uintptr_t stack_base_addr,
                             uint32_t stack_limit_bytes){

  uint8_t stack_marker = 0U;
  int8_t ret = 0;

  s_deepest_depth = current_depth;

  uintptr_t current_addr = (uintptr_t)&stack_marker;
  uint32_t stack_used = 0U;

  if (stack_base_addr >= current_addr) {
    stack_used = (uint32_t)(stack_base_addr - current_addr);
  }

  (void)printf("[Depth %2u] stack_marker addr: %p, stack used: %4u bytes\n",
               current_depth, (void *)&stack_marker, stack_used);

  if (stack_used >= stack_limit_bytes) {
    (void)printf("[Depth %2u] WARNING: Stack usage (%u bytes) exceeds limit! Aborting recursion.\n",
                 current_depth, stack_used);
    ret = -1;
  }
  else if (current_depth >= max_depth) {
    ret = 0;
  }
  else {
    ret = recurse_with_monitor(current_depth + 1U, max_depth, stack_base_addr, stack_limit_bytes);
  }

  return ret;
}

int main(void) {

  uint8_t stack_base_marker = 0U;
  const uintptr_t stack_base_addr = (uintptr_t)&stack_base_marker;
  int8_t ret = 0;

  (void)printf("=== Stack Depth Monitor (limit: %u bytes) ===\n", STACK_LIMIT_BYTES_DEFAULT);

  ret = recurse_with_monitor(0U, MAX_RECURSION_DEPTH, stack_base_addr, STACK_LIMIT_BYTES_DEFAULT);

  if (0 == ret) {
    (void)printf("Result: %d (max depth reached)\n", ret);
  } else {
    (void)printf("Result: %d (stack limit reached at depth %u)\n", ret, s_deepest_depth);
  }

  return 0;
}