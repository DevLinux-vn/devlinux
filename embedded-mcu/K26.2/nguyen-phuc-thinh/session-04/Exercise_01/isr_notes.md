# ISR Notes - Session 04

## 1. Why must a flag shared between the ISR and a task be `volatile`?
The compiler's optimizer assumes that if a variable is not modified by the code within a loop (like `while (!flag)`), its value won't ever change. Therefore, it optimizes the code by caching the variable's value in a CPU register. If an ISR modifies the variable in RAM, the task will never see the change because it only reads the cached register, resulting in an infinite loop. Declaring the flag as `volatile` strictly forces the compiler to fetch the variable from memory (RAM) on every single read, preventing this caching behavior.

## 2. Why is calling `ESP_LOGI()` or `vTaskDelay()` from inside `button_isr()` dangerous?
ISRs execute in a high-priority hardware context that pre-empts all regular tasks. `vTaskDelay()` and `ESP_LOGI()` are blocking FreeRTOS API functions (for example, `ESP_LOGI` relies on mutexes and UART transmission time). Calling a blocking function from an ISR violates RTOS principles—it causes the CPU to halt inside the interrupt context, which can instantly crash the system, trigger a Watchdog Timer (WDT) reset, or cause a priority deadlock. An ISR must strictly use non-blocking, interrupt-safe APIs (those ending in `...FromISR`, like `xQueueSendFromISR`).

## 3. Compare your two implementations.
**Responsiveness:** The interrupt-driven implementation (Session 04) is significantly more responsive. Its worst-case latency is determined by hardware interrupt dispatch time (usually a few microseconds). In contrast, polling (Session 03) can have a delay up to the length of the polling period (e.g., 5ms).

**CPU Usage:** While the button is untouched, the interrupt version uses virtually 0% CPU. The task cleanly blocks indefinitely on the queue (`xQueueReceive`), allowing the RTOS idle task to run and save power. Polling, however, constantly wakes the CPU to read the register.

**Ease of Implementation:** The polling version was much easier to get right. Handling mechanical bounce in a polling loop is as simple as waiting and reading the pin again. With interrupts, every single bounce fires an interrupt event, forcing us to convert a simple state check into complex timestamp math and queue timeout logic to correctly reassemble the hardware state.