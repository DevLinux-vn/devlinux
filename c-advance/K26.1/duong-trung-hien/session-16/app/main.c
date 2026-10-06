#include "alert.h"
#include "app_fsm.h"
#include "cli_cmd.h"
#include "common_types.h"
#include "config.h"
#include "data_proc.h"
#include "hal_sim.h"
#include "hal_timer.h"
#include "logger.h"
#include "sensor_mgr.h"
#include "telemetry.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define APP_SUCCESS (0)
#define APP_FAILURE (1)

#define APP_CONFIG_FILE "config.txt"
#define APP_LOG_FILE "iot_monitor.log"

#define APP_INPUT_BUFFER_SIZE (64U)
#define APP_FILTER_WINDOW_SIZE (10U)

static bool app_is_command(const char *p_input, const char *p_command);

int main(void) {
  alert_config_t config;
  data_proc_t temp_filter;
  data_proc_t humi_filter;
  sensor_data_t sensor_data;
  uint8_t telemetry_buffer[TELEMETRY_SERIALIZED_SIZE];
  uint32_t telemetry_written = 0U;

  char input_buffer[APP_INPUT_BUFFER_SIZE];

  system_state_t state = SYS_INIT;
  cli_context_t cli_context;
  bool running = true;
  status_t status;

  (void)printf("=== IoT Environmental Monitoring System v1.0.0 ===\n");

  logger_init(LOG_LEVEL_INFO, APP_LOG_FILE);
  LOG_INFO("%s", "Logger initialized: iot_monitor.log");

  hal_sim_init(42U);
  LOG_INFO("%s", "HAL initialized (seed=42)");

  hal_timer_init();

  status = config_load(APP_CONFIG_FILE, &config);

  if (status != STATUS_OK) {
    LOG_ERROR("%s", "Config initialization failed");
    state = fsm_run(state, EVT_INIT_FAIL);
  }

  if (state != SYS_ERROR) {
    alert_init(&config);

    status = sensor_mgr_init();

    if (status != STATUS_OK) {
      LOG_ERROR("Sensor manager initialization failed: %d", (int)status);
      state = fsm_run(state, EVT_INIT_FAIL);
    } else {
      LOG_INFO("%s", "Sensors initialized: Temperature, Humidity");
    }
  }

  if (state != SYS_ERROR) {
    status = data_proc_init(&temp_filter, APP_FILTER_WINDOW_SIZE);

    if (status != STATUS_OK) {
      LOG_ERROR("Temperature filter initialization failed: %d", (int)status);
      state = fsm_run(state, EVT_INIT_FAIL);
    }
  }

  if (state != SYS_ERROR) {
    status = data_proc_init(&humi_filter, APP_FILTER_WINDOW_SIZE);

    if (status != STATUS_OK) {
      LOG_ERROR("Humidity filter initialization failed: %d", (int)status);
      state = fsm_run(state, EVT_INIT_FAIL);
    }
  }

  if (state != SYS_ERROR) {
    state = fsm_run(state, EVT_INIT_OK);
  }

  cli_context.p_state = &state;

  while (running && (state != SYS_ERROR)) {
    system_event_t monitoring_event = EVT_NUM_EVENTS;

    (void)printf("IoT> ");
    (void)fflush(stdout);

    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
      running = false;
      continue;
    }

    status = cli_dispatch(input_buffer, &cli_context);

    if (status != STATUS_OK) {
      continue;
    }

    /*
     * The existing CLI handler does not expose EVT_CMD_QUIT
     * to main(), so detect the successfully dispatched command
     * here and terminate before another monitoring cycle.
     */
    if (app_is_command(input_buffer, "quit")) {
      running = false;
      continue;
    }

    if (app_is_command(input_buffer, "alert_ack")) {
      alert_acknowledge();
    }

    if (state == SYS_MONITORING) {
      hal_sim_update();

      status = sensor_mgr_read_all(&sensor_data);

      if (status != STATUS_OK) {
        LOG_ERROR("Sensor read failed: %d", (int)status);
        monitoring_event = EVT_SENSOR_FAIL;
      } else {
        status = data_proc_add_sample(&temp_filter, sensor_data.temperature);

        if (status != STATUS_OK) {
          LOG_ERROR("Temperature filter failed: %d", (int)status);
          monitoring_event = EVT_SENSOR_FAIL;
        }

        if (monitoring_event == EVT_NUM_EVENTS) {
          status = data_proc_add_sample(&humi_filter, sensor_data.humidity);

          if (status != STATUS_OK) {
            LOG_ERROR("Humidity filter failed: %d", (int)status);
            monitoring_event = EVT_SENSOR_FAIL;
          }
        }
      }

      if (monitoring_event == EVT_NUM_EVENTS) {
        const int16_t temperature_average = data_proc_get_average(&temp_filter);

        const int16_t humidity_average = data_proc_get_average(&humi_filter);

        sensor_data.temperature = temperature_average;
        sensor_data.humidity = humidity_average;

        if (alert_check(&sensor_data)) {
          monitoring_event = EVT_ALERT_TRIGGERED;
        }
      }

      if (monitoring_event == EVT_NUM_EVENTS) {
        status =
            telemetry_serialize(&sensor_data, telemetry_buffer,
                                sizeof(telemetry_buffer), &telemetry_written);

        if (status != STATUS_OK) {
          LOG_ERROR("Telemetry serialization failed: %d", (int)status);
          monitoring_event = EVT_SENSOR_FAIL;
        }
      }

      if (monitoring_event == EVT_NUM_EVENTS) {
        LOG_INFO("Monitoring... Temp=%d.%dC Humi=%d.%d%% "
                 "(tick=%u)",
                 (int)(sensor_data.temperature / 10),
                 (int)(sensor_data.temperature % 10),
                 (int)(sensor_data.humidity / 10),
                 (int)(sensor_data.humidity % 10),
                 (unsigned int)sensor_data.timestamp);

        LOG_DEBUG("Telemetry serialized: %u bytes",
                  (unsigned int)telemetry_written);
      }

      if (monitoring_event != EVT_NUM_EVENTS) {
        state = fsm_run(state, monitoring_event);
      }

      hal_timer_tick();
    }
  }

  logger_close();

  (void)printf("[INFO] Shutting down. Goodbye!\n");

  return (state == SYS_ERROR) ? APP_FAILURE : APP_SUCCESS;
}

static bool app_is_command(const char *p_input, const char *p_command) {
  char command[APP_INPUT_BUFFER_SIZE];
  size_t length;

  if ((p_input == NULL) || (p_command == NULL)) {
    return false;
  }

  length = strlen(p_input);

  if (length >= sizeof(command)) {
    return false;
  }

  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  (void)memcpy(command, p_input, length + 1U);

  while ((length > 0U) &&
         ((command[length - 1U] == '\n') || (command[length - 1U] == '\r') ||
          (command[length - 1U] == ' ') || (command[length - 1U] == '\t'))) {
    command[length - 1U] = '\0';
    --length;
  }

  return (strcmp(command, p_command) == 0);
}