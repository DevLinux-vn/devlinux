#include <stdio.h>
#include <time.h>
#include "logger.h"

void log_timestamp(void)
{
    FILE *fp = fopen("app.log", "a");

    if (fp == NULL)
    {
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d\n",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec);

    fclose(fp);
}

void log_write(const char *msg)
{
    FILE *fp = fopen("app.log", "a");

    if (fp == NULL)
    {
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec,
            msg);

    fclose(fp);
}

void log_error(const char *msg)
{
    FILE *fp = fopen("app.log", "a");

    if (fp == NULL)
    {
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] [ERROR] %s\n",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec,
            msg);

    fclose(fp);
}