# isr\_notes.md — Session 04

## 1\. Why must a flag shared between an ISR and a task be `volatile`?

Because without it the compiler is entitled to assume the variable cannot change
between two accesses in the same straight-line piece of code, and it will use
that assumption to delete the reload.

Concretely, given:

```c
static bool flag;           /\* no volatile \*/
while (!flag) { }
```

the compiler sees a loop whose body writes nothing, and a condition that reads a
variable nothing in the loop modifies. It is allowed to hoist the load out of
the loop entirely, producing the equivalent of:

```c
bool tmp = flag;
while (!tmp) { }            /\* loads once, then spins on a register forever \*/
```

At `-O2` this is a routine and entirely legal transformation. The same applies
to `if (flag)` checks in a loop: the value gets cached in a register across
iterations, and the register is never refreshed from memory.

The ISR writing the variable does not prevent this, and this is the part that
surprises people. The compiler's model of the program is the C abstract machine,
in which nothing executes except the code it can see reaching this point. An ISR
is not called from anywhere in the source — it is invoked by hardware through a
vector table. From the compiler's point of view it is dead code that nobody
calls, so it cannot possibly modify anything the loop depends on. The write is
real at runtime and invisible at compile time.

`volatile` fixes exactly this: it tells the compiler that every read in the
source must become an actual load from memory, and every write an actual store,
in program order, with no caching, no merging, and no elimination.

What `volatile` does **not** give you is atomicity or ordering with respect to
other variables. A `volatile int64\_t` on a 32-bit core is still two stores, and
an ISR can land between them. That is precisely why this project passes button
events through a FreeRTOS queue rather than through a shared struct: the
timestamp and the edge direction must travel together or not at all, and
`xQueueSendFromISR()` gives that guarantee where `volatile` cannot.

## 2\. Why is calling `ESP\_LOGI()` or `vTaskDelay()` from inside the ISR dangerous?

The mechanism is **blocking in a context that has no task to block**.

FreeRTOS's blocking primitives work by taking the calling task's control block
off the ready list, putting it on a delayed or waiting list, and running the
scheduler to pick someone else. An ISR is not a task. It has no TCB, it runs on
the interrupt stack, and it interrupted whatever task happened to be running —
a task that has done nothing wrong and is not the one asking to wait. There is
nothing coherent for the kernel to suspend. `vTaskDelay()` from an ISR trips a
`configASSERT` in a debug build and corrupts the scheduler's lists in a release
build.

`ESP\_LOGI()` reaches the same failure by a longer road. It formats through
`vprintf` and writes to the UART, and that path takes a lock on the stdout
stream. If the interrupted task was already holding that lock, the ISR waits for
a lock that can only be released by a task that cannot run until the ISR
returns — a deadlock with the CPU pinned. Even when the lock is free, the write
is milliseconds long with interrupts of equal and lower priority masked for the
duration, which shows up as dropped edges and jitter everywhere else in the
system.

There is a third failure mode specific to the ESP32 family and worth naming
because it is the one that produces the most baffling crash reports. Code in
flash is reached through a cache, and that cache is disabled during SPI flash
writes (NVS commits, OTA). An ISR marked `IRAM\_ATTR` lives in internal RAM and
survives that window, but any function it *calls* must also be IRAM-resident.
`ESP\_LOGI()` is not. Call it from an ISR and the program works perfectly for
weeks, then panics with `Cache disabled but cached memory region accessed` the
first time a write to flash coincides with a button press.

Hence the rule the exercise imposes: timestamp, capture the edge, queue it,
return. All logging in this project happens in `gesture\_task`.

## 3\. Comparing the two implementations

**Responsiveness.** The interrupt version wins, but the honest margin is smaller
than it first looks. The polling loop sampled every `POLL\_PERIOD\_MS` = 10 ms, so
an edge landing just after a sample waited a full 10 ms before being noticed —
and the debounce filter needed the level to hold steady for `DEBOUNCE\_MS` = 25 ms
on top of that. Worst case from physical contact to a decoded edge was roughly
35 ms. The ISR version sees the edge in microseconds: interrupt latency on the
ESP32-S3 is on the order of 2–3 µs, plus queue handling and a context switch,
call it under 50 µs. But the 25 ms debounce wait is still there, because it is a
property of the switch, not of the detection method. So the real end-to-end
improvement is about 10 ms out of 35 — meaningful for the *precision* of the
timestamps, largely invisible to a human finger.

Where it matters more is timing accuracy. In the polling version every
measurement was quantised to 10 ms, so the boundary between "click" and "long
press" was fuzzy by that much. Now the timestamp is taken in the ISR at the
moment of the edge, and the gesture thresholds are compared against real
microsecond values.

**CPU while idle.** The interrupt version wins decisively. The polling loop woke
100 times a second forever, whether or not anyone touched the button — a hundred
context switches, a hundred register reads, a hundred passes through the state
machine, all to conclude nothing happened. The gesture task here blocks on
`xQueueReceive()` with `portMAX\_DELAY` whenever there is no pending click and no
hold in progress, which means the task is genuinely off the ready list and the
idle task can let the CPU enter a low-power state. On battery this is the
difference that matters, not the 10 ms.

**Which was easier to get right — honestly, the polling version.** By a clear
margin, and the reason is instructive.

Polling gives you the button's *state* on every pass. State is self-correcting:
if a sample is missed or misread, the next pass 10 ms later reads the truth
again and the state machine heals itself. There is no concurrency at all —
one loop, one thread, no shared data, nothing to protect.

Interrupts give you *events*, and events are lossy and unforgiving. Miss one and
the state machine is permanently wrong until the next edge happens to correct
it. Along with that come problems that simply did not exist before: the queue
can overflow during a bad bounce burst and needs depth chosen for that; the ISR
must be IRAM-resident and must not touch anything that isn't; shared state needs
a safe hand-off mechanism; and `portYIELD\_FROM\_ISR()` has to be handled or the
response is silently delayed to the next tick.

The part that actually had to be redesigned was the long-press repeat. In the
polling version it came free — the loop ran anyway, so checking "has 500 ms
elapsed" cost nothing. With interrupts, holding the button generates *no edges*,
so the ISR has nothing to report and the task would sleep straight through the
entire hold. The fix was to stop blocking forever and instead compute, in every
state, how long until the next thing is due, and pass that as the
`xQueueReceive()` timeout. That one function, `next\_deadline\_ticks()`, is the
only genuinely new logic in the whole rewrite.

What survived unchanged is the gesture decoder itself: press and release edges,
the pending-click buffer, the double-click window, the rule that a long press
must not also emit a click on release. That logic never cared where the edges
came from — which is a decent argument for having written it as a separate
concern in the first place.

