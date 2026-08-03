#include "logger.h"
#include <stdio.h>
#include <time.h>

static void append_to_file(const char *msg) {
    FILE *f = fopen("app.log", "a");
    if (f) {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
}

void log_write(const char *msg) {
    append_to_file(msg);
}

void log_timestamp(void) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "[INFO] Timestamp: %Y-%m-%d %H:%M:%S", t);
    append_to_file(buf);
}

void log_error(const char *msg) {
    char buf[1024];
    snprintf(buf, sizeof(buf), "[ERROR] %s", msg);
    append_to_file(buf);
}
