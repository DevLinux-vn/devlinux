/**
 * @file main.c
 * @brief Application demonstrating Command Dispatcher pattern.
 */

#include "command_dispatcher.h"

/**
 * @brief Main entry point executing command dispatch verification.
 * @return 0 on success.
 */
int main(void)
{
    /* Dispatch 5 valid commands */
    Dispatch_Command("LED_ON");
    Dispatch_Command("LED_OFF");
    Dispatch_Command("MOTOR_START");
    Dispatch_Command("MOTOR_STOP");
    Dispatch_Command("STATUS");

    /* Dispatch 1 unknown command to verify rejection */
    Dispatch_Command("REBOOT");

    return 0;
}
