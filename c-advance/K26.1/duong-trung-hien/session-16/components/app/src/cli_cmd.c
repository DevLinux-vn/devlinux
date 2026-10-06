#include "cli_cmd.h"
#include "app_fsm.h"
#include "common_types.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static void cmd_start(void *p_ctx);
static void cmd_stop(void *p_ctx);
static void cmd_status(void *p_ctx);
static void cmd_config(void *p_ctx);
static void cmd_alert_ack(void *p_ctx);
static void cmd_reset(void *p_ctx);
static void cmd_help(void *p_ctx);
static void cmd_quit(void *p_ctx);

static const cli_command_t s_cmd_table[] = {
    {"start", cmd_start},         {"stop", cmd_stop},
    {"status", cmd_status},       {"config", cmd_config},
    {"alert_ack", cmd_alert_ack}, {"reset", cmd_reset},
    {"help", cmd_help},           {"quit", cmd_quit},
};

status_t cli_dispatch(const char *p_input, void *p_ctx) {
  if ((NULL == p_input) || (NULL == p_ctx)) {
    return STATUS_ERR_NULL_PTR;
  }

  char command[32U];
  const size_t input_length = strlen(p_input);

  if (input_length >= sizeof(command)) {
    return STATUS_ERR_INVALID_PARAM;
  }

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  (void)memcpy(command, p_input, input_length + 1U);

  while ((input_length > 0U) && ((command[input_length - 1U] == '\n') ||
                                 (command[input_length - 1U] == '\r'))) {
    command[input_length - 1U] = '\0';
  }

  for (size_t index = 0U; index < ARRAY_SIZE(s_cmd_table); ++index) {
    if (0 == strcmp(command, s_cmd_table[index].p_name)) {
      s_cmd_table[index].handler(p_ctx);
      return STATUS_OK;
    }
  }

  (void)printf("Unknown command\n");
  return STATUS_ERR_INVALID_PARAM;
}

static void cmd_start(void *p_ctx) {
  cli_context_t *const p_cli_ctx = (cli_context_t *)p_ctx;

  if (NULL == p_cli_ctx->p_state) {
    return;
  }

  *p_cli_ctx->p_state = fsm_run(*p_cli_ctx->p_state, EVT_CMD_START);
}

static void cmd_stop(void *p_ctx) {
  cli_context_t *const p_cli_ctx = (cli_context_t *)p_ctx;

  if (NULL == p_cli_ctx->p_state) {
    return;
  }

  *p_cli_ctx->p_state = fsm_run(*p_cli_ctx->p_state, EVT_CMD_STOP);
}

// cppcheck-suppress constParameterPointer
static void cmd_status(void *p_ctx) {
  const cli_context_t *const p_cli_ctx = (const cli_context_t *)p_ctx;

  if ((NULL == p_cli_ctx) || (NULL == p_cli_ctx->p_state)) {
    return;
  }

  (void)printf("State: %d\n", (int)*p_cli_ctx->p_state);
}

static void cmd_config(void *p_ctx) {
  (void)p_ctx;
  (void)printf("Configuration command\n");
}

static void cmd_alert_ack(void *p_ctx) {
  cli_context_t *const p_cli_ctx = (cli_context_t *)p_ctx;

  if (NULL == p_cli_ctx->p_state) {
    return;
  }

  *p_cli_ctx->p_state = fsm_run(*p_cli_ctx->p_state, EVT_ALERT_ACK);
}

static void cmd_reset(void *p_ctx) {
  cli_context_t *const p_cli_ctx = (cli_context_t *)p_ctx;

  if (NULL == p_cli_ctx->p_state) {
    return;
  }

  *p_cli_ctx->p_state = fsm_run(*p_cli_ctx->p_state, EVT_CMD_RESET);
}

static void cmd_help(void *p_ctx) {
  (void)p_ctx;

  (void)printf(
      "Commands: start stop status config alert_ack reset help quit\n");
}

static void cmd_quit(void *p_ctx) {
  cli_context_t *const p_cli_ctx = (cli_context_t *)p_ctx;

  if (NULL == p_cli_ctx->p_state) {
    return;
  }

  *p_cli_ctx->p_state = fsm_run(*p_cli_ctx->p_state, EVT_CMD_QUIT);
}