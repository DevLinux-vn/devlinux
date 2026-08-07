#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Function pointer type for a command handler.
 *
 * A command handler takes no parameters and returns no value.
 */
typedef void (*cmd_action_t)(void);

/**
 * @brief Entry in the command dispatch table.
 */
typedef struct
{
    const char *p_command_str; /**< Null-terminated command string. */
    cmd_action_t action;       /**< Function pointer to invoke.     */
} command_entry_t;

/**
 * @brief Calculate the number of elements in a fixed-size array.
 *
 * @param arr Array whose number of elements is required.
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/**
 * @brief Turn the LED on.
 */
static void Cmd_LED_On(void);

/**
 * @brief Turn the LED off.
 */
static void Cmd_LED_Off(void);

/**
 * @brief Start the motor.
 */
static void Cmd_Motor_Start(void);

/**
 * @brief Stop the motor.
 */
static void Cmd_Motor_Stop(void);

/**
 * @brief Print the current system status.
 */
static void Cmd_Status(void);

/**
 * @brief Dispatch a received ASCII command string to its handler.
 *
 * The function searches the static command table for a command whose
 * string matches @p p_received_cmd. If a match is found, the associated
 * handler function is invoked.
 *
 * @param[in] p_received_cmd Null-terminated command string.
 *
 * @note A NULL input pointer is detected before strcmp() is called.
 */
static void Dispatch_Command(const char *p_received_cmd);

/**
 * @brief Constant command dispatch table.
 *
 * Each command string is associated with exactly one handler function.
 * The table is declared static const so it cannot be modified at runtime.
 */
static const command_entry_t CMD_TABLE[] =
{
    { "LED_ON",      Cmd_LED_On      },
    { "LED_OFF",     Cmd_LED_Off     },
    { "MOTOR_START", Cmd_Motor_Start },
    { "MOTOR_STOP",  Cmd_Motor_Stop  },
    { "STATUS",      Cmd_Status      }
};

/**
 * @brief Turn the LED on.
 */
static void Cmd_LED_On(void)
{
    (void)printf("[CMD] LED turned ON.\n");
}

/**
 * @brief Turn the LED off.
 */
static void Cmd_LED_Off(void)
{
    (void)printf("[CMD] LED turned OFF.\n");
}

/**
 * @brief Start the motor.
 */
static void Cmd_Motor_Start(void)
{
    (void)printf("[CMD] Motor started at 1500 RPM.\n");
}

/**
 * @brief Stop the motor.
 */
static void Cmd_Motor_Stop(void)
{
    (void)printf("[CMD] Motor stopped.\n");
}

/**
 * @brief Print the current system status.
 */
static void Cmd_Status(void)
{
    (void)printf("[CMD] System status: OK.\n");
}

/**
 * @brief Dispatch a received ASCII command string to its handler.
 *
 * @param[in] p_received_cmd Null-terminated command string.
 */
static void Dispatch_Command(const char *p_received_cmd)
{
    size_t index;

    if (p_received_cmd == NULL)
    {
        (void)printf("[CMD] Unknown command: NULL\n");
        return;
    }

    for (index = 0U; index < ARRAY_SIZE(CMD_TABLE); ++index)
    {
        if (strcmp(p_received_cmd, CMD_TABLE[index].p_command_str) == 0)
        {
            CMD_TABLE[index].action();
            return;
        }
    }

    (void)printf("[CMD] Unknown command: %s\n", p_received_cmd);
}

/**
 * @brief Program entry point.
 *
 * Tests all supported commands and one unsupported command.
 *
 * @return 0 on successful completion.
 */
int main(void)
{
    Dispatch_Command("LED_ON");
    Dispatch_Command("LED_OFF");
    Dispatch_Command("MOTOR_START");
    Dispatch_Command("MOTOR_STOP");
    Dispatch_Command("STATUS");
    Dispatch_Command("REBOOT");

    return 0;
}