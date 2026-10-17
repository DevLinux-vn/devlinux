#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define ERROR                   (-2)
#define PARSE_SUCCESS           (0)
#define PARSE_FAILED            (-1)
#define FIND_DELIMITER_SUCCESS  (0)
#define FIND_DELIMITER_FAILED   (-1)
#define STRING_SIMILAR          (0)

/**
 * @brief Parses a command string and generates a formatted response.
 *
 * The command is parsed according to the expected command format, and the
 * corresponding response is written to the provided output buffer.
 *
 * @param[in]  p_cmd      Null-terminated input command string.
 * @param[out] p_response Buffer used to store the formatted response.
 * @param[in]  resp_size  Size of the response buffer in bytes.
 *
 * @return 0 on successful parsing and response generation.
 * @return -1 if the command is invalid or cannot be parsed.
 */
int8_t parse_command(const char *p_cmd, char *p_response, size_t resp_size);

/**
 * @brief Extracts the device name from a command string.
 *
 * Searches for the command delimiter and copies the device name into the
 * provided output buffer.
 *
 * @param[in]  p_cmd       Null-terminated input command string.
 * @param[out] p_device    Buffer used to store the extracted device name.
 * @param[in]  device_size Size of the device buffer in bytes.
 *
 * @return 0 on successful extraction.
 * @return -1 if the input is invalid, the delimiter is not found,
 *         or the output buffer is too small.
 */
int8_t extract_device_name(const char *p_cmd, char *p_device, size_t device_size);

#endif /* COMMAND_PARSER_H */