/**
 * @file logger.c
 * @brief Safe logging utility with error checking for embedded systems.
 */

#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

static void write_to_file(const char *prefix, const char *msg) {
    FILE *log_file = fopen("app.log", "a");
    /* Defensive check: Prevent NULL pointer dereference if file system is full/read-only */
    if (!log_file) {
        perror("Error opening app.log");
        return;
    }

    time_t raw_time;
    struct tm *time_info;
    char time_buffer[20];

    time(&raw_time);
    time_info = localtime(&raw_time);
    
    if (time_info == NULL) {
        strcpy(time_buffer, "UNKNOWN_TIME");
    } else {
        /* Check strftime return value to guarantee buffer safety */
        size_t ret = strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", time_info);
        if (ret == 0) {
            strcpy(time_buffer, "UNKNOWN_TIME");
        }
    }

    if (prefix) {
        fprintf(log_file, "[%s] %s %s\n", time_buffer, prefix, msg);
    } else {
        fprintf(log_file, "[%s] %s\n", time_buffer, msg);
    }

    fclose(log_file);
}

void log_write(const char *msg) {
    write_to_file(NULL, msg);
}

void log_timestamp(void) {
    write_to_file(NULL, "Timestamp triggered.");
}

void log_error(const char *msg) {
    write_to_file("[ERROR]", msg);
}