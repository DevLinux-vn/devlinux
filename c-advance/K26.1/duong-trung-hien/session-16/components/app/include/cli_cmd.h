#ifndef CLI_CMD_H
#define CLI_CMD_H

#include "app_fsm.h"
#include "common_types.h"

/**
 * @brief CLI command handler function pointer.
 *
 * @param[in,out] p_ctx CLI context.
 */
typedef void (*cli_command_handler_t)(void *p_ctx);

/**
 * @brief CLI command table entry.
 */
typedef struct {
  const char *p_name;
  cli_command_handler_t handler;
} cli_command_t;

/**
 * @brief CLI runtime context.
 */
typedef struct {
  system_state_t *p_state;
} cli_context_t;

/**
 * @brief Dispatch a CLI command.
 *
 * @param[in] p_input Null-terminated command string.
 * @param[in,out] p_ctx CLI context.
 *
 * @return STATUS_OK when a command is dispatched successfully.
 *         STATUS_ERR_NULL_PTR when an argument is NULL.
 *         STATUS_ERR_INVALID_PARAM when the command is unknown.
 */
status_t cli_dispatch(const char *p_input, void *p_ctx);

#endif /* CLI_CMD_H */