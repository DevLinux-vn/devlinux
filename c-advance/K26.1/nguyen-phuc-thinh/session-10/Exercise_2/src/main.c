/**
 * @file main.c
 * @brief Demonstration of the mini command parser.
 */
#include <stdio.h>
#include "command_parser.h"

/**
 * @brief Main execution entry point.
 * 
 * @return int Exit code.
 */
int main(void) 
{
    printf("========== L10 Lab 2: String Toolkit & Parsing ==========\n\n");
    printf("--- Command Parser ---\n");

    const char *commands[] = {
        "LED:ON",
        "MOTOR:START:500",
        "TEMP:READ",
        "LED:OFF",
        "MOTOR:STOP"
    };
    
    size_t num_cmds = sizeof(commands) / sizeof(commands[0]);

    for (size_t i = 0U; i < num_cmds; i++) 
    {
        char response[128];
        /* Call the safe parsing function */
        parse_command(commands[i], response, sizeof(response));
        
        /* Formatted output matching the requirement */
        printf("Raw: %-16s -> Formatted Response: %s\n", commands[i], response);
    }

    return 0;
}