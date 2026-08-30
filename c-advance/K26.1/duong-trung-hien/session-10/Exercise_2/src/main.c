#include "command_parser.h"
#include <stdio.h>

#define APP_SUCCESS     (0)
#define BUFFER_SIZE     (512)

int32_t main() {
    const char *commands[] = {
    "LED:ON",
    "MOTOR:START:500",
    "TEMP:READ",
    "LED:OFF",
    "MOTOR:STOP"
};
    char des[BUFFER_SIZE];
    size_t command_count = sizeof(commands) / sizeof(commands[0]);

    for (uint32_t i = 0U; i < command_count; i++) {
        if (parse_command(commands[i], des, BUFFER_SIZE) == PARSE_SUCCESS) {
            printf("Raw: %-18s -> Formatted Response: %s\n", commands[i], des);
        }
    }

    return APP_SUCCESS;
}