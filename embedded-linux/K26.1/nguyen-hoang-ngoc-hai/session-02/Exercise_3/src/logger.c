#include <stdio.h>
#include <time.h>
#include "logger.h"

static void write_current_timestamp(FILE *file) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(file, "%s", buffer);
}

void log_write(const char *msg) {
    FILE *file = fopen("app.log", "a");

    if (file == NULL) {
        return;
    }

    write_current_timestamp(file);
    fprintf(file, " %s\n", msg);

    fclose(file);
}

void log_timestamp(void) {
    FILE *file = fopen("app.log", "a");

    if (file == NULL) {
        return;
    }

    write_current_timestamp(file);
    fprintf(file, "\n");

    fclose(file);
}

void log_error(const char *msg) {
    FILE *file = fopen("app.log", "a");

    if (file == NULL) {
        return;
    }

    write_current_timestamp(file);
    fprintf(file, " [ERROR] %s\n", msg);

    fclose(file);
}