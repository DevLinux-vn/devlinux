#include "logger.h"
#include <stdio.h>
#include <time.h>

void log_timestamp(char *buf, int size) {
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", lt);
}

static void write_log(const char *prefix, const char *msg) {
    FILE *f = fopen("app.log", "a");
    if (!f) return;
    char ts[32];
    log_timestamp(ts, sizeof(ts));
    fprintf(f, "%s %s%s\n", ts, prefix, msg);
    fclose(f);
}

void log_info(const char *msg)  { write_log("",        msg); }
void log_error(const char *msg) { write_log("[ERROR] ", msg); }
