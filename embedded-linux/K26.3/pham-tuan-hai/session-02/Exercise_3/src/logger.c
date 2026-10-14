#include "logger.h"
#include <stdio.h>
#include <time.h>

void log_write(const char *msg) {
    FILE *file = fopen("app.log", "a");
    if (file == NULL) return;

    fprintf(file, "%s\n", msg);
    fclose(file);
}

void log_timestamp(void) {
    FILE *file = fopen("app.log", "a");
    if (file == NULL) return;

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    fprintf(file, "[TIME] %s\n", buffer);
    fclose(file);
}

void log_error(const char *msg) {
    FILE *file = fopen("app.log", "a");
    if (file == NULL) return;

    fprintf(file, "[ERROR] %s\n", msg);
    fclose(file);
}
