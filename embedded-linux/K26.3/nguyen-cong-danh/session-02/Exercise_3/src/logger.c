#include <stdio.h>
#include <time.h>
#include "logger.h"

#define LOG_FILE "app.log"

static FILE* open_log(void)
{
    return fopen(LOG_FILE, "a");
}

void log_timestamp(void)
{
    FILE *fp = open_log();

    if (fp == NULL)
    {
        return;
    }

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    char timestamp[32];

    strftime(timestamp,
             sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S",
             tm_info);

    fprintf(fp, "[%s]\n", timestamp);

    fclose(fp);
}

void log_write(const char *msg)
{
    FILE *fp = open_log();

    if (fp == NULL)
    {
        return;
    }

    fprintf(fp, "%s\n", msg);

    fclose(fp);
}

void log_error(const char *msg)
{
    FILE *fp = open_log();

    if (fp == NULL)
    {
        return;
    }

    fprintf(fp, "[ERROR] %s\n", msg);

    fclose(fp);
}