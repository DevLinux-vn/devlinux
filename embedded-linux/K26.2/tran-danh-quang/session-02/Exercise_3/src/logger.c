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
    if (fclose(f) < 0) {
        perror("fclose");
        return;
    }
}

void log_write(const char *msg) {
    append_to_file(msg);
}

void log_timestamp(void) {
    time_t now = time(NULL);
    if (now == (time_t)(-1)) {
        perror("time");
        append_to_file("[ERROR] time() failed");
        return;
    }
    struct tm *t = localtime(&now);
    if (!t) {
        perror("localtime");  /* print to stderr first */
        append_to_file("[ERROR] localtime failed");
        return;
    }
    char buf[128];
    strftime(buf, sizeof(buf), LOG_TIMESTAMP_PREFIX "%Y-%m-%d %H:%M:%S", t);
    append_to_file(buf);
}

void log_error(const char *msg) {
    char buf[1024];
    int ret = snprintf(buf, sizeof(buf), "[ERROR] %s", msg);
    if (ret < 0 || ret >= (int)sizeof(buf)) {
        perror("snprintf");
        return;
    }
    append_to_file(buf);
}
