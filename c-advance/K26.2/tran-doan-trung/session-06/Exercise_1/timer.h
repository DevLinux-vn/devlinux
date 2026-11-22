/**
 * @file timer.h
 * @brief Software Timer with Callback Registration Pattern Interface.
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Function pointer type for timer expiry callbacks.
 */
typedef void (*timer_callback_t)(void);

/**
 * @brief Query whether a timer is currently active.
 * @return true if a timer is running, false otherwise.
 */
bool Timer_IsRunning(void);

/**
 * @brief Register a callback to fire after a given number of ticks.
 * @details Stores the expiry tick count and callback pointer in the internal timer state.
 *          If a timer is already running, registration is rejected with a warning.
 * @param[in] expire_at_tick Tick number at which the callback fires.
 * @param[in] p_callback     Function pointer to invoke. Must not be NULL.
 */
void Timer_Register(uint32_t expire_at_tick, timer_callback_t p_callback);

/**
 * @brief Advance the timer by one tick. Fires callback if expired.
 * @details Increments current tick count and checks if expiry threshold is reached.
 *          Guards callback invocation against NULL pointer dereference.
 */
void Timer_Tick(void);

/**
 * @brief Cancel any active timer and reset internal state to initial values.
 */
void Timer_Reset(void);

#endif /* TIMER_H */
