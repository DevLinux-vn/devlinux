#include "logger.h"
#include <stdio.h>
#include <time.h>
void log_write(const char *msg)
{
    FILE *f = fopen("app.log", "a");
    if (f != NULL)
    {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
}
void log_timestamp(void)
{
    FILE *f = fopen("app.log", "a");

    if (f == NULL)
    {
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(f,
            "%04d-%02d-%02d %02d:%02d:%02d\n",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec);
    fclose(f);
}

void log_error(const char *msg)
{
    FILE *f = fopen("app.log", "a");

    if (f == NULL)
    {
        return;
    }

    fprintf(f, "[ERROR] %s\n", msg);

    fclose(f);
}