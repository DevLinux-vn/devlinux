#include "logger.h"
#include <stdio.h>
#include <time.h>

static void write_to_file(const char *prefix, const char *msg) {
    FILE *f = fopen("app.log", "a");
    if (f) {
        if (prefix) {
            fprintf(f, "%s %s\n", prefix, msg);
        } else {
            fprintf(f, "%s\n", msg);
        }
        fclose(f);
    }
}

void log_write(const char *msg) {
    write_to_file("[INFO]", msg);
}

void log_timestamp(void) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    
    // Yêu cầu: in ra log (hoặc terminal)
    FILE *f = fopen("app.log", "a");
    if (f) {
        fprintf(f, "[TIME] %s\n", buf);
        fclose(f);
    }
    printf("[TIME] %s\n", buf);
}

void log_error(const char *msg) {
    write_to_file("[ERROR]", msg);
}