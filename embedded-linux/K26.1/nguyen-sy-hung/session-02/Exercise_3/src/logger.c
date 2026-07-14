#include <stdio.h>
#include <time.h>
#include "logger.h"

void log_write(const char *msg)
{
    FILE *file = fopen("app.log", "a");

    if (file == NULL)
    {
        return;
    }

    fprintf(file, "%s\n", msg);

    fclose(file);
}


void log_timestamp(void)
{
    FILE *file = fopen("app.log", "a");

    if (file == NULL)
    {
        return;
    }

    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);

    fprintf(file,
            "%04d-%02d-%02d %02d:%02d:%02d\n",
            time_info->tm_year + 1900,
            time_info->tm_mon + 1,
            time_info->tm_mday,
            time_info->tm_hour,
            time_info->tm_min,
            time_info->tm_sec);

    fclose(file);
}


void log_error(const char *msg)
{
    FILE *file = fopen("app.log", "a");

    if (file == NULL)
    {
        return;
    }

    fprintf(file, "[ERROR] %s\n", msg);

    fclose(file);
}