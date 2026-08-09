#include "logger.h"
#include <stdio.h>
#include <time.h>

void log_timestamp(void)
{
    time_t now;
    struct tm *time_info;

    time(&now);
    time_info = localtime(&now);

    printf("%04d-%02d-%02d %02d:%02d:%02d\n",
           time_info->tm_year + 1900,
           time_info->tm_mon + 1,
           time_info->tm_mday,
           time_info->tm_hour,
           time_info->tm_min,
           time_info->tm_sec);
}

void log_write(const char *msg)
{
    FILE *file = fopen("app.log", "a");

    if (file == NULL)
    {
        return;
    }

    time_t now;
    struct tm *time_info;

    time(&now);
    time_info = localtime(&now);

    fprintf(file,
            "%04d-%02d-%02d %02d:%02d:%02d %s\n",
            time_info->tm_year + 1900,
            time_info->tm_mon + 1,
            time_info->tm_mday,
            time_info->tm_hour,
            time_info->tm_min,
            time_info->tm_sec,
            msg);

    fclose(file);
}

void log_error(const char *msg)
{
    FILE *file = fopen("app.log", "a");

    if (file == NULL)
    {
        return;
    }

    time_t now;
    struct tm *time_info;

    time(&now);
    time_info = localtime(&now);

    fprintf(file,
            "%04d-%02d-%02d %02d:%02d:%02d [ERROR] %s\n",
            time_info->tm_year + 1900,
            time_info->tm_mon + 1,
            time_info->tm_mday,
            time_info->tm_hour,
            time_info->tm_min,
            time_info->tm_sec,
            msg);

    fclose(file);
}