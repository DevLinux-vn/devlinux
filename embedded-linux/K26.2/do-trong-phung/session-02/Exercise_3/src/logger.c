#include "logger.h"
#include <stdio.h>
#include <time.h>

void log_timestamp(void)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    if (t != NULL)
    {
        printf("%04d-%02d-%02d %02d:%02d:%02d",
               t->tm_year + 1900,
               t->tm_mon + 1,
               t->tm_mday,
               t->tm_hour,
               t->tm_min,
               t->tm_sec);
    }
}

void log_write(const char *msg)
{
    FILE *file = fopen("app.log", "a");

    if (file == NULL)
    {
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    if (t != NULL)
    {
        fprintf(file,
                "%04d-%02d-%02d %02d:%02d:%02d %s\n",
                t->tm_year + 1900,
                t->tm_mon + 1,
                t->tm_mday,
                t->tm_hour,
                t->tm_min,
                t->tm_sec,
                msg);
    }

    fclose(file);
}

void log_error(const char *msg)
{
    char error_msg[512];

    snprintf(error_msg, sizeof(error_msg), "[ERROR] %s", msg);

    log_write(error_msg);
}