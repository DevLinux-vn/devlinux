/**
 * @file    main.c
 * @brief   Demo program: parse a set of sample UART-style commands.
 */
#include <stdio.h>
#include "command_parser.h"

/**
 * @brief   Program entry point.
 *
 * @return  0 on success.
 */
int main(void)
{
    static const char *commands[] = {
        "LED:ON",
        "MOTOR:START:500",
        "TEMP:READ",
        "LED:OFF",
        "MOTOR:STOP"
    };
    static const size_t num_commands = sizeof(commands) / sizeof(commands[0]);
    char response[64];
    size_t i;

    printf("========== L10 Lab 2: String Toolkit & Parsing ==========\n\n");
    printf("--- Command Parser ---\n");

    for (i = 0U; i < num_commands; i++)
    {
        (void)parse_command(commands[i], response, sizeof(response));
        printf("Raw: %-16s -> Formatted Response: %s\n", commands[i], response);
    }

    return 0;
}