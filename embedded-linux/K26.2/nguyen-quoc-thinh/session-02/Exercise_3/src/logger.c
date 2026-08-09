#include "logger.h"
#include <stdio.h>
#include <time.h>

#define LOG_FILE "app.log"

static void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

void log_write(const char *msg) {
    char ts[32];
    get_timestamp(ts, sizeof(ts));

    FILE *f = fopen(LOG_FILE, "a");
    if (f == NULL) {
        return;
    }
    fprintf(f, "[%s] %s\n", ts, msg);
    fclose(f);
}

void log_timestamp(void) {
    char ts[32];
    get_timestamp(ts, sizeof(ts));
    printf("%s\n", ts);
}

void log_error(const char *msg) {
    char ts[32];
    get_timestamp(ts, sizeof(ts));

    FILE *f = fopen(LOG_FILE, "a");
    if (f == NULL) {
        return;
    }
    fprintf(f, "[%s] [ERROR] %s\n", ts, msg);
    fclose(f);
}
