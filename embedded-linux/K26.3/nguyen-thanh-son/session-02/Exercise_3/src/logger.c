#define _POSIX_C_SOURCE 200809L   /* localtime_r */

#include <stdio.h>
#include <time.h>
#include "logger.h"

#define TS_LEN 20   /* "YYYY-MM-DD HH:MM:SS" + '\0' */

static void get_timestamp(char *buf, size_t size)
{
    time_t now = time(NULL);
    struct tm tm_now;

    if (localtime_r(&now, &tm_now) == NULL ||
        strftime(buf, size, "%Y-%m-%d %H:%M:%S", &tm_now) == 0)
    {
        snprintf(buf, size, "0000-00-00 00:00:00");
    }
}

static void append_entry(const char *level, const char *msg)
{
    char ts[TS_LEN];
    FILE *fp = fopen(LOG_FILE, "a");

    if (fp == NULL)
    {
        perror("fopen " LOG_FILE);
        return;
    }

    get_timestamp(ts, sizeof(ts));
    fprintf(fp, "[%s] %s%s\n", ts, level, msg ? msg : "(null)");
    fclose(fp);
}

void log_write(const char *msg)
{
    append_entry("", msg);
}

void log_timestamp(void)
{
    char ts[TS_LEN];

    get_timestamp(ts, sizeof(ts));
    printf("%s\n", ts);
}

void log_error(const char *msg)
{
    append_entry("[ERROR] ", msg);
}
