#include <stdio.h>
#include <time.h>
#include "logger.h"

void log_timestamp(void)
{
    time_t now;
    struct tm *time_info;
    char buffer[20];

    time(&now);
    time_info = localtime(&now);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

    printf("%s", buffer);
}

void log_write(const char *msg)
{
    FILE *file;

    file = fopen("app.log", "a");

    if (file == NULL)
    {
        return;
    }

    time_t now;
    struct tm *time_info;
    char buffer[20];

    time(&now);
    time_info = localtime(&now);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

    fprintf(file, "[%s] %s\n", buffer, msg);

    fclose(file);
}

void log_error(const char *msg)
{
    FILE *file;

    file = fopen("app.log", "a");

    if (file == NULL)
    {
        return;
    }

    time_t now;
    struct tm *time_info;
    char buffer[20];

    time(&now);
    time_info = localtime(&now);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

    fprintf(file, "[%s] [ERROR] %s\n", buffer, msg);

    fclose(file);
}
