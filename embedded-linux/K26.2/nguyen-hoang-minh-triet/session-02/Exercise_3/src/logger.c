```c
#include "logger.h"

#include <stdio.h>
#include <time.h>

#define base_tm_year 1900
#define base_tm_mon 1
#define base_tm_mday 1
#define base_tm_hour 0
#define base_tm_min 0
#define base_tm_sec 0

#define LOG_FILE "app.log"

static int write_timestamp(FILE *file)
{
    time_t current_time;
    struct tm *time_info;

    current_time = time(NULL);

    if (current_time == (time_t)-1)
    {
        return 0;
    }

    time_info = localtime(&current_time);

    if (time_info == NULL)
    {
        return 0;
    }

    if (fprintf(file,
                "%04d-%02d-%02d %02d:%02d:%02d",
                time_info->tm_year + base_tm_year,
                time_info->tm_mon + base_tm_mon,
                time_info->tm_mday + base_tm_mday,
                time_info->tm_hour + base_tm_hour,
                time_info->tm_min + base_tm_min,
                time_info->tm_sec + base_tm_sec) < 0)
    {
        return 0;
    }

    return 1;
}

void log_write(const char *msg)
{
    FILE *file;

    file = fopen(LOG_FILE, "a");

    if (file == NULL)
    {
        fprintf(stderr, "Error: cannot open %s\n", LOG_FILE);
        return;
    }

    if (write_timestamp(file) == 0)
    {
        fprintf(stderr, "Error: cannot write timestamp\n");
    }
    else if (fprintf(file, " %s\n", msg) < 0)
    {
        fprintf(stderr, "Error: cannot write log message\n");
    }

    if (fclose(file) != 0)
    {
        fprintf(stderr, "Error: cannot close %s\n", LOG_FILE);
    }
}

void log_timestamp(void)
{
    FILE *file;

    file = fopen(LOG_FILE, "a");

    if (file == NULL)
    {
        fprintf(stderr, "Error: cannot open %s\n", LOG_FILE);
        return;
    }

    if (write_timestamp(file) == 0)
    {
        fprintf(stderr, "Error: cannot write timestamp\n");
    }
    else if (fprintf(file, "\n") < 0)
    {
        fprintf(stderr, "Error: cannot write newline\n");
    }

    if (fclose(file) != 0)
    {
        fprintf(stderr, "Error: cannot close %s\n", LOG_FILE);
    }
}

void log_error(const char *msg)
{
    FILE *file;

    file = fopen(LOG_FILE, "a");

    if (file == NULL)
    {
        fprintf(stderr, "Error: cannot open %s\n", LOG_FILE);
        return;
    }

    if (write_timestamp(file) == 0)
    {
        fprintf(stderr, "Error: cannot write timestamp\n");
    }
    else if (fprintf(file, " [ERROR] %s\n", msg) < 0)
    {
        fprintf(stderr, "Error: cannot write error message\n");
    }

    if (fclose(file) != 0)
    {
        fprintf(stderr, "Error: cannot close %s\n", LOG_FILE);
    }
}

