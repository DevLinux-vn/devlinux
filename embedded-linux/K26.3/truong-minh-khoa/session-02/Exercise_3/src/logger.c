#include "logger.h"
#include <stdio.h>
#include <time.h>

static const char *LOG_FILE = "app.log";

void log_write(const char *msg) {
    FILE *f = fopen(LOG_FILE, "a");
    if (f) {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
}

void log_timestamp(void) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);

    FILE *f = fopen(LOG_FILE, "a");
    if (f) {
        fprintf(f, "%s\n", buf);
        fclose(f);
    }
}

void log_error(const char *msg) {
    FILE *f = fopen(LOG_FILE, "a");
    if (f) {
        fprintf(f, "[ERROR] %s\n", msg);
        fclose(f);
    }
}

