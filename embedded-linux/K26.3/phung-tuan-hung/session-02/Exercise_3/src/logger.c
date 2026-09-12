#include "logger.h"
#include <stdio.h>
#include <time.h>

#define LOG_FILE "app.log"

/* Helper: fill buf with current time as "YYYY-MM-DD HH:MM:SS" */
static void current_time_str(char *buf, size_t size)
{
    time_t now = time(NULL);
    struct tm *lt = localtime(&now);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", lt);
}

/* Print the current timestamp to stdout */
void log_timestamp(void)
{
    char ts[32];
    current_time_str(ts, sizeof(ts));
    printf("%s\n", ts);
}

/* Append a normal log entry (with timestamp) to app.log */
void log_write(const char *msg)
{
    char ts[32];
    FILE *f = fopen(LOG_FILE, "a");
    if (f == NULL) {
        return;
    }
    current_time_str(ts, sizeof(ts));
    fprintf(f, "[%s] %s\n", ts, msg);
    fclose(f);
}

/* Append an error log entry prefixed with [ERROR] to app.log */
void log_error(const char *msg)
{
    char ts[32];
    FILE *f = fopen(LOG_FILE, "a");
    if (f == NULL) {
        return;
    }
    current_time_str(ts, sizeof(ts));
    fprintf(f, "[%s] [ERROR] %s\n", ts, msg);
    fclose(f);
}
