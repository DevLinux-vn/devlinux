/**
 * @file command_parser.h
 * @brief Command string parsing utilities for embedded systems.
 */
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Extracts the device name from a command string.
 *
 * @param[in] p_cmd       The raw command string.
 * @param[out] p_device   Buffer to hold the extracted device name.
 * @param[in] device_size Size of the device buffer.
 * @return 0 on success, -1 if no delimiter found or invalid arguments.
 */
int8_t extract_device_name(const char *p_cmd, char *p_device, size_t device_size);

/**
 * @brief Parses a command string and generates a response.
 *
 * @param[in] p_cmd       The raw command string (e.g., "LED:ON").
 * @param[out] p_response Buffer to hold the formatted response.
 * @param[in] resp_size   Size of the response buffer.
 * @return 0 on success, -1 on parse failure.
 */
int8_t parse_command(const char *p_cmd, char *p_response, size_t resp_size);

#endif /* COMMAND_PARSER_H */