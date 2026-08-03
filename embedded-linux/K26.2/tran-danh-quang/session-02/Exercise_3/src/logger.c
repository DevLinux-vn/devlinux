#include "logger.h"
#include <stdio.h>
#include <time.h>

#define LOG_TIMESTAMP_PREFIX "[INFO] Timestamp: "

static void append_to_file(const char *msg) {
    FILE *f = fopen("app.log", "a");
    if (!f) {
        perror("fopen");
        return;
    }
    if (fprintf(f, "%s\n", msg) < 0) {
        perror("fprintf");
        fclose(f);
        return;
    }
    fclose(f);
}

void log_write(const char *msg) {
    append_to_file(msg);
}

void log_timestamp(void) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (!t) {
        append_to_file("[ERROR] localtime failed");
        return;
    }
    char buf[128];
    strftime(buf, sizeof(buf), LOG_TIMESTAMP_PREFIX "%Y-%m-%d %H:%M:%S", t);
    append_to_file(buf);
}

void log_error(const char *msg) {
    char buf[1024];
    snprintf(buf, sizeof(buf), "[ERROR] %s", msg);
    append_to_file(buf);
}
