#include "logger.h"

#include <stdio.h>
#include <time.h>

void log_timestamp(void)
{
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);

    if (local_time == NULL)
    {
        return;
    }

    printf("%04d-%02d-%02d %02d:%02d:%02d",
           local_time->tm_year + 1900,
           local_time->tm_mon + 1,
           local_time->tm_mday,
           local_time->tm_hour,
           local_time->tm_min,
           local_time->tm_sec);
}

static void get_timestamp(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);

    if (local_time == NULL)
    {
        snprintf(buffer, size, "0000-00-00 00:00:00");
        return;
    }

    if (strftime(buffer, size, "%Y-%m-%d %H:%M:%S", local_time) == 0)
    {
        snprintf(buffer, size, "0000-00-00 00:00:00");
    }
}

void log_write(const char *msg)
{
    FILE *file = fopen("app.log", "a");

    if (file == NULL)
    {
        return;
    }

    char timestamp[32];

    get_timestamp(timestamp, sizeof(timestamp));

    fprintf(file, "[%s] %s\n", timestamp, msg);

    fclose(file);
}

void log_error(const char *msg)
{
    FILE *file = fopen("app.log", "a");

    if (file == NULL)
    {
        return;
    }

    char timestamp[32];

    get_timestamp(timestamp, sizeof(timestamp));

    fprintf(file, "[%s] [ERROR] %s\n", timestamp, msg);

    fclose(file);
}