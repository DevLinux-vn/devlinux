/**
 * @file command_dispatcher.c
 * @brief Implementation of Command Dispatcher Jump Table.
 */

#include "command_dispatcher.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Action handler for "LED_ON" command.
 */
static void Cmd_LED_On(void)
{
    printf("[CMD] LED turned ON.\n");
}

/**
 * @brief Action handler for "LED_OFF" command.
 */
static void Cmd_LED_Off(void)
{
    printf("[CMD] LED turned OFF.\n");
}

/**
 * @brief Action handler for "MOTOR_START" command.
 */
static void Cmd_Motor_Start(void)
{
    printf("[CMD] Motor started at 1500 RPM.\n");
}

/**
 * @brief Action handler for "MOTOR_STOP" command.
 */
static void Cmd_Motor_Stop(void)
{
    printf("[CMD] Motor stopped.\n");
}

/**
 * @brief Action handler for "STATUS" command.
 */
static void Cmd_Status(void)
{
    printf("[CMD] System status: OK.\n");
}

/**
 * @brief Static const command jump table stored in Flash memory (.rodata).
 */
static const command_entry_t CMD_TABLE[] = {
    { "LED_ON",      Cmd_LED_On      },
    { "LED_OFF",     Cmd_LED_Off     },
    { "MOTOR_START", Cmd_Motor_Start },
    { "MOTOR_STOP",  Cmd_Motor_Stop  },
    { "STATUS",      Cmd_Status      }
};

void Dispatch_Command(const char *p_received_cmd)
{
    /* MISRA Directive 4.11 / CERT EXP34-C: Guard against NULL pointer dereference */
    if (NULL == p_received_cmd) {
        return;
    }

    const size_t cmd_count = ARRAY_SIZE(CMD_TABLE);
    for (size_t i = 0U; i < cmd_count; ++i) {
        if (strcmp(p_received_cmd, CMD_TABLE[i].p_command_str) == 0) {
            if (CMD_TABLE[i].action != NULL) {
                CMD_TABLE[i].action();
            }
            return;
        }
    }

    /* Command not found in table */
    printf("[CMD] Unknown command: %s\n", p_received_cmd);
}
