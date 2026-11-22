/**
 * @file command_dispatcher.h
 * @brief Command Dispatcher Interface using String-Driven Jump Table.
 */

#ifndef COMMAND_DISPATCHER_H
#define COMMAND_DISPATCHER_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Macro calculating the number of elements in a static array.
 * @details Idiomatic pattern used in Linux kernel and Zephyr RTOS for compile-time bound calculation.
 */
#define ARRAY_SIZE(arr)  (sizeof(arr) / sizeof((arr)[0]))

/**
 * @brief Function pointer type for command action handler functions.
 */
typedef void (*cmd_action_t)(void);

/**
 * @brief Command jump table entry structure mapping command string to action handler.
 */
typedef struct {
    const char   *p_command_str; /**< Null-terminated command string keyword. */
    cmd_action_t  action;        /**< Function pointer to invoke on match.    */
} command_entry_t;

/**
 * @brief Dispatch a received ASCII command string to its registered handler.
 * @details Traverses the command lookup table, matches command string via strcmp,
 *          and invokes the corresponding callback. Prints an error if unknown.
 * @param[in] p_received_cmd Null-terminated command string. Must not be NULL.
 */
void Dispatch_Command(const char *p_received_cmd);

#endif /* COMMAND_DISPATCHER_H */
