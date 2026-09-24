#include <stdint.h>
#include <inttypes.h>
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
int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth,
                             const uintptr_t stack_base_addr,
                             uint32_t stack_limit_bytes)
{
   uint8_t stack_marker    = 0;
   uint8_t *p_stack_marker = &stack_marker; // Pointer to the top of the current stack frame.

   uintptr_t current_stack_usage = stack_base_addr - (uintptr_t)p_stack_marker;
   if (current_stack_usage > stack_limit_bytes)
   {
      printf("[Depth %2" PRIu32 "] WARNING: Stack usage (%" PRIuPTR" bytes) exceeds limit! Aborting recursion.\n",
               current_depth, current_stack_usage);
      return -1; // Stack limit exceeded. 
   }
   
   printf("[Depth %2" PRIu32 "] stack_marker addr: %p, stack used: %4" PRIuPTR " bytes\n",
           current_depth, (void *)&stack_marker, current_stack_usage);
           
   if (current_depth < max_depth)
   {
      return recurse_with_monitor(current_depth + 1, max_depth, stack_base_addr, stack_limit_bytes);
   }
   else
   {
      return 0; // Reached max depth successfully.
   }
}

int main(void)
{
   uint8_t stack_marker       = 0;
   int8_t result              = 0;
   uintptr_t stack_base_addr  = (uintptr_t)&stack_marker; // Capture the base address of the stack.
   uint32_t max_depth         = 100;                      // Set a maximum recursion depth.
   uint32_t stack_limit_bytes = 4096;                     // Set a stack usage limit in bytes.

   printf("=== Stack Depth Monitor (limit: %" PRIu32 " bytes) ===\n", stack_limit_bytes);

   result = recurse_with_monitor(0, max_depth, stack_base_addr, stack_limit_bytes);
   if (result == 0)
   {
      printf("Result: 0 (max depth reached)\n");
   }
   else
   {
      printf("Result: -1 (stack limit reached)\n");
   }
   return result;
}