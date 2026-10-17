#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef void (*cmd_action_t)(void);

typedef struct
{
    const char *p_command_str; /**< Command keyword string. */
    cmd_action_t action; /**< Handler function. */
} command_entry_t;

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

static void Cmd_LED_On(void);
static void Cmd_LED_Off(void);
static void Cmd_Motor_Start(void);
static void Cmd_Motor_Stop(void);
static void Cmd_Status(void);

static const command_entry_t CMD_TABLE[] = {
    { "LED_ON",      Cmd_LED_On      },
    { "LED_OFF",     Cmd_LED_Off     },
    { "MOTOR_START", Cmd_Motor_Start },
    { "MOTOR_STOP",  Cmd_Motor_Stop  },
    { "STATUS",      Cmd_Status      },
};

#define CMD_TABLE_SIZE (ARRAY_SIZE(CMD_TABLE))

static void Cmd_LED_On(void){
    (void)printf("[CMD] LED turned ON.\n");
}

static void Cmd_LED_Off(void){
    (void)printf("[CMD] LED turned OFF.\n");
}

static void Cmd_Motor_Start(void){
    (void)printf("[CMD] Motor started at 1500 RPM.\n");
}

static void Cmd_Motor_Stop(void){
    (void)printf("[CMD] Motor stopped.\n");
}

static void Cmd_Status(void){
    (void)printf("[CMD] System status: OK.\n");
}

/**
 * @brief Dispatch a received ASCII command string to its handler.
 * @param[in] p_received_cmd  Null-terminated command string. Must not be NULL.
 */
void Dispatch_Command(const char *p_received_cmd){
    uint32_t idx;
    uint8_t matched = 0U;
    if (p_received_cmd == NULL)
    {
        (void)printf("[CMD] Unknown command: (null)\n");
        return;
    }
    for (idx = 0U; idx < CMD_TABLE_SIZE; idx++)
    {
        if (strcmp(CMD_TABLE[idx].p_command_str, p_received_cmd) == 0)
        {
            CMD_TABLE[idx].action();
            matched = 1U;
            break;
        }
    }
    if (matched == 0U)
    {
        (void)printf("[CMD] Unknown command: %s\n",p_received_cmd );  
    }
}

/**
 * @brief Program entry point. Runs all 5 valid commands plus one unknown.
 * @return 0 on success.
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