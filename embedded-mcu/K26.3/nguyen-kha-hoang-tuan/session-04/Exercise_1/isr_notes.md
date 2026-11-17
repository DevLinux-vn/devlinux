### Questions

Answer in `isr_notes.md`, one short paragraph each:

1. Why must the shared button-state variable be `volatile`? What could happen if the main loop reused a cached value?
2. Why should the ISR only save state and timestamps? Why do logging and delays belong in the main loop, and why is `vTaskDelay()` invalid inside the ISR?
3. Compare polling and interrupts: edge-detection latency, time until the display updates, CPU usage while idle, and implementation difficulty. State your timing assumptions; interrupts do not automatically make the display respond faster.
//--------------------------------------------------------------------------------------------
### 1. Why the shared button-state variable must be volatile

- Prevents compiler caching: volatile tells the compiler that the variable is updated outside the main loop (by the ISR), forcing the CPU to read the fresh value from RAM every time.
- Avoids optimization bugs: Without volatile, the compiler sees that app_main() never modifies the variable itself and may keep a single cached copy in a CPU register.
- What happens if cached: If app_main() reuses an old cached value, it will never see when the ISR changes the button state—causing the program to completely miss button presses or get stuck in one state forever.

### 2. Why the ISR only saves state/timestamps, and why vTaskDelay() is invalid in an ISR

- Keep the ISR fast: An ISR pauses the main program and blocks other interrupts while running, so it must finish in microseconds by only recording the pin level and timestamp.
- Why logging and delays belong in app_main(): printf() uses slow serial buffers and locks, and delays freeze the processor—doing either inside an ISR can miss new button edges or trigger a watchdog crash.
- Why vTaskDelay() is invalid in an ISR: vTaskDelay() tells the FreeRTOS scheduler to put the currently running task to sleep. Because an ISR is a hardware interrupt and not a FreeRTOS task, it cannot sleep, and calling vTaskDelay() inside an ISR causes a system crash (kernel panic).

### 3. Polling vs. Interrupts Comparison

(Timing assumptions: 10 ms main-loop tick, 25 ms debounce delay, 350 ms double-click window, and 800 ms long-press threshold)

- Edge-detection latency:
    - Interrupts: Almost instant (microseconds), triggered by hardware the exact moment the button is pressed or released.
    - Polling: Delayed by 0–10 ms (since it only checks once every 10 ms tick) and can miss very brief presses.
- Time until the display updates:
    - Nearly identical for both (~375 ms after release for single-click, ~25 ms after 2nd press for double-click, ~800 ms for long-press): Interrupts do not make the display update faster because the program still has to wait for the 25 ms debounce, the 350 ms / 800 ms gesture timers, and the 10 ms main-loop check.
- CPU usage while idle:
    - Interrupts: Can be 0% if the task sleeps until an interrupt wakes it (or < 1% in our design since app_main() wakes briefly every 10 ms to check timestamps).
    - Polling: Wakes up every 10 ms to read the GPIO pin continuously, even when nobody touches the button.
- Implementation difficulty:
    - Polling: Easy—uses simple top-to-bottom code with no shared-memory bugs.
    - Interrupts: Harder—requires IRAM_ATTR, volatile variables, critical sections (portENTER_CRITICAL) to prevent corrupted 64-bit timestamps, and a non-blocking state machine.
