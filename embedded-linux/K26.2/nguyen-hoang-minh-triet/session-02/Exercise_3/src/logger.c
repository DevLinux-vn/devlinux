#include <stdio.h>
#include <time.h>

#include "logger.h"

#define LOG_FILE "app.log"


static void write_timestamp(FILE *file) {
    time_t current_time;
    struct tm *time_info;

    current_time = time(NULL);
    time_info = localtime(&current_time);

    if (time_info != NULL)
    {
        fprintf(
            file,
            "%04d-%02d-%02d %02d:%02d:%02d",
            time_info->tm_year + 1900,
            time_info->tm_mon + 1,
            time_info->tm_mday,
            time_info->tm_hour,
            time_info->tm_min,
            time_info->tm_sec
        );
    }
}

void log_write(const char *msg) {
    FILE *file;

    file = fopen(LOG_FILE, "a");

    if (file == NULL)
    {
        return;
    }

    write_timestamp(file);
    fprintf(file, " %s\n", msg);

    fclose(file);
}

void log_timestamp(void)    {
    FILE *file;

    file = fopen(LOG_FILE, "a");

    if (file == NULL)
    {
        return;
    }

    write_timestamp(file);
    fprintf(file, "\n");

    fclose(file);
}

void log_error(const char *msg) {
    FILE *file;

    file = fopen(LOG_FILE, "a");

    if (file == NULL)
    {
        return;
    }

    write_timestamp(file);
    fprintf(file, " [ERROR] %s\n", msg);

    fclose(file);
}
