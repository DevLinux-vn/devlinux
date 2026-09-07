/**
 * @file    command_parser.h
 * @brief   Public interface for parsing simple "DEVICE:ARGS" style
 *          command strings, as commonly received over UART/BLE/etc.
 */
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief   Parse a command string and produce a human-readable
 *          response string.
 *
 * @param[in]  p_cmd      The raw command string (e.g. "LED:ON").
 *                         May be NULL.
 * @param[out] p_response Buffer to receive the formatted, null-
 *                         terminated response. May be NULL only if
 *                         @p resp_size is 0.
 * @param[in]  resp_size  Size of @p p_response, in bytes.
 *
 * @return  0 on success, -1 on parse failure (invalid input,
 *          unknown device, or malformed arguments).
 */
int8_t parse_command(const char *p_cmd, char *p_response, size_t resp_size);

/**
 * @brief   Extract the device name (the text before the first ':')
 *          from a command string.
 *
 * @param[in]  p_cmd       The raw command string. May be NULL.
 * @param[out] p_device    Buffer to receive the extracted,
 *                          null-terminated device name.
 * @param[in]  device_size Size of @p p_device, in bytes.
 *
 * @return  0 on success, -1 if no ':' delimiter is found, the
 *          device name would not fit in @p p_device, or any
 *          argument is invalid.
 */
int8_t extract_device_name(const char *p_cmd, char *p_device, size_t device_size);

#endif /* COMMAND_PARSER_H */