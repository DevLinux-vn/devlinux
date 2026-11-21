# ISR Notes

## 1. Why must the button-state variable be volatile?

The button state is written by the ISR and read by the main loop.
The compiler cannot see that link, because no code in the loop writes it.

Without `volatile`, the compiler may load the variable once into a register
and reuse that cached value on every iteration.
The main loop would then never notice a press or release recorded by the ISR,
and the gesture logic would hang.

`volatile` forces a fresh read from memory each time.
It does not make multi-field or 64-bit accesses atomic,
so the level and both timestamps are copied inside a critical section (spinlock)
to get a consistent snapshot.

## 2. Why should the ISR only save state and timestamps?

An ISR preempts whatever is running and must finish in microseconds.
Long work delays other interrupts and the scheduler.

Logging, display writes and decoding can block, take locks,
or call code that is not ISR-safe, which can deadlock or crash.
So the ISR only reads the level, timestamps the edge and stores them.
Decoding, logging and display updates run in the main loop.

`vTaskDelay()` is invalid in an ISR because it blocks the calling task,
and an ISR is not a task.
There is no task context to suspend, and the scheduler cannot switch away from it.

## 3. Polling vs interrupts

**Assumptions:** polling period 10 ms, debounce 25 ms, double-click window 350 ms.

- **Edge-detection latency:** polling notices an edge 0-10 ms late (5 ms on average).
  The interrupt timestamps it within microseconds.
- **Time until the display updates:** about the same in both designs.
  It is dominated by debounce (about 25-35 ms) and, for a single click,
  by the 350 ms double-click wait, because a single click cannot be committed earlier.
  The interrupt version still decodes gestures in a 10 ms loop,
  so the display does not respond faster; only the timestamps are more accurate.
- **CPU usage while idle:** nearly the same, because the main loop still wakes every 10 ms.
  Interrupts would save power only if the loop could sleep until an edge arrives.
- **Implementation difficulty:** polling is simpler.
  Interrupts add `IRAM_ATTR`, shared-state synchronisation and race conditions to reason about.