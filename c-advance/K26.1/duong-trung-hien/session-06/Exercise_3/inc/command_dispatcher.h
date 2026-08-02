#ifndef COMMAND_DISPATCHER_H
#define COMMAND_DISPATCHER_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief Calculates the number of elements in an array.
 *
 * This macro returns the total number of elements in a statically
 * allocated array by dividing the total size of the array by the
 * size of a single element.
 *
 * @note This macro must only be used with actual arrays. Using it
 *       with a pointer will produce an incorrect result.
 *
 * @param[in] arr Statically allocated array.
 *
 * @return Number of elements in the array as a uint32_t.
 */
#define ARRAY_SIZE(arr) (uint32_t)(sizeof(arr) / sizeof((arr)[0]))

/**
 * @brief Dispatches a received command to its corresponding handler.
 *
 * This function compares the received command string against the
 * registered command table. If a matching command is found, the
 * associated handler function is invoked.
 *
 * @param[in] p_receive_cmd Pointer to a null-terminated command string.
 *
 * @note If the input pointer is NULL, the command string is empty,
 *       or no matching command is found, no handler is executed.
 */
void Dispatch_Command(const char *p_receive_cmd);

#endif /* COMMAND_DISPATCHER_H */