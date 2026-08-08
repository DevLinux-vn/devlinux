#include <stdio.h>
#include <time.h>
#include "logger.h"

static void get_timestamp(char *buffer, size_t size)
{
    time_t now;
    struct tm *time_info;

    now = time(NULL);
    time_info = localtime(&now);

    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", time_info);
}

void log_write(const char *msg)
{
    FILE *file;
    char timestamp[20];

    get_timestamp(timestamp, sizeof(timestamp));

    file = fopen("app.log", "a");

    if (file == NULL)
        return;

    fprintf(file, "[%s] %s\n", timestamp, msg);

    fclose(file);
}

void log_timestamp(void)
{
    char timestamp[20];

    get_timestamp(timestamp, sizeof(timestamp));

    printf("%s\n", timestamp);
}

void log_error(const char *msg)
{
    FILE *file;
    char timestamp[20];

    get_timestamp(timestamp, sizeof(timestamp));

    file = fopen("app.log", "a");

    if (file == NULL)
        return;

    fprintf(file, "[%s] [ERROR] %s\n", timestamp, msg);

    fclose(file);
}