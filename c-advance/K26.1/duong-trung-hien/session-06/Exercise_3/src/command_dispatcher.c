#include "command_dispatcher.h"

#define ARRAY_SAME      (0U)

/**
 * @brief Function pointer type for command handler functions.
 */
typedef void (*cmd_action_t)(void);

/**
 * @brief Command table entry.
 *
 * This structure maps a command string to its corresponding
 * handler function. It is used by the command dispatcher to
 * locate and execute the appropriate action for a received
 * command.
 */
typedef struct {
    const char      *p_command_str; /**< Command keyword string. */
    cmd_action_t    action;         /**< Handler function.       */
} command_entry_t;

/**
 * @brief Turn on the LED.
 */
static void Cmd_LED_On(void);

/**
 * @brief Turn off the LED.
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
 * @brief Display the current system status.
 */
static void Cmd_Status(void);

static const command_entry_t CMD_TABLE[] = {
    { "LED_ON",      Cmd_LED_On      },
    { "LED_OFF",     Cmd_LED_Off     },
    { "MOTOR_START", Cmd_Motor_Start },
    { "MOTOR_STOP",  Cmd_Motor_Stop  },
    { "STATUS",      Cmd_Status      },
};

static void Cmd_LED_On(void)
{
    printf("[CMD] LED turned ON.\n");
} 

static void Cmd_LED_Off(void)
{
    printf("[CMD] LED turned OFF.\n");
} 

static void Cmd_Motor_Start(void)
{
    printf("[CMD] Motor started at 1500 RPM.\n");
} 

static void Cmd_Motor_Stop(void)
{
    printf("[CMD] Motor stopped.\n");
} 

static void Cmd_Status(void)
{
    printf("[CMD] System status: OK.\n");
} 

void Dispatch_Command(const char *p_receive_cmd)
{
    if ((NULL == p_receive_cmd) || ('\0' == *p_receive_cmd))
    {
        return;
    }

    for (uint32_t i = 0U; i < ARRAY_SIZE(CMD_TABLE); i++)
    {
        if (strcmp(p_receive_cmd, CMD_TABLE[i].p_command_str) == ARRAY_SAME)
        {
            CMD_TABLE[i].action();
            return;
        }
    }

    printf("[CMD] Unknown command: %s\n", p_receive_cmd);
}


