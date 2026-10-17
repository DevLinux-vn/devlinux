#include "logger.h"

#include <stdio.h>
#include <time.h>

#define LOG_FILE "app.log"

static void write_current_timestamp(FILE *file)
{
    time_t now;
    struct tm *local_time;
    char buffer[20];

    now = time(NULL);
    local_time = localtime(&now);
    if (local_time == NULL)
    {
        fprintf(file, "0000-00-00 00:00:00");
        return;
    }

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local_time);
    fprintf(file, "%s", buffer);
}

void log_write(const char *msg)
{
    FILE *file = fopen(LOG_FILE, "a");

    if (file == NULL)
    {
        return;
    }

    write_current_timestamp(file);
    fprintf(file, " %s\n", msg);
    fclose(file);
}

void log_timestamp(void)
{
    FILE *file = fopen(LOG_FILE, "a");

    if (file == NULL)
    {
        return;
    }

    write_current_timestamp(file);
    fprintf(file, "\n");
    fclose(file);
}

void log_error(const char *msg)
{
    FILE *file = fopen(LOG_FILE, "a");

    if (file == NULL)
    {
        return;
    }

    write_current_timestamp(file);
    fprintf(file, " [ERROR] %s\n", msg);
    fclose(file);
}
