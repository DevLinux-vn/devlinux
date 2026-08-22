#include <stdio.h>
#include <time.h>
#include "../include/logger.h"

void log_write(const char *msg)
{
    printf("[INFO] %s\n", msg);
}

void log_timestamp(void)
{
    time_t now;
    struct tm *time_info;
    char timestamp[20];

    time(&now);

    time_info = localtime(&now);

    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time_info);

    printf("%s\n", timestamp);
}

void log_error(const char *msg)
{
    fprintf(stderr, "[ERROR] %s \n", msg);
}