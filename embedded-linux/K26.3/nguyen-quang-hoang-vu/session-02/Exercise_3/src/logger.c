#include "logger.h"
#include <stdio.h>
#include <time.h>

void log_write(const char *msg) {
    FILE *f = fopen("app.log", "a");
    if (f == NULL) return;
    fprintf(f, "%s\n", msg);
    fclose(f);
}

void log_timestamp(void) {
    FILE *f = fopen("app.log", "a");
    if (f == NULL) return;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", t);
    fprintf(f, "[%s]\n", buffer);
    fclose(f);
}

void log_error(const char *msg) {
    FILE *f = fopen("app.log", "a");
    if (f == NULL) return;
    fprintf(f, "[ERROR] %s\n", msg);
    fclose(f);
}
