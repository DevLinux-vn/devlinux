#include <stdio.h>
#include <string.h>
#include <time.h>
#include "logger.h"

static FILE *log_file = NULL;
#define LOG_FILE "app.log"

static void write_current_timestamp(void) {
    if (log_file == NULL)
        return;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);

    if (fprintf(log_file, "%s", buffer) < 0) {
        perror("fprintf");
        return;
    }
}

void log_init(void)
{
    if (log_file != NULL)
        return;

    log_file = fopen(LOG_FILE, "a");

    if (log_file == NULL)
        perror("fopen");
}

void log_write(const char *msg) {
    if (log_file == NULL) {
        return;
    }

    write_current_timestamp();

    fprintf(log_file, " %s\n", msg);

    fflush(log_file);
}

void log_timestamp(void) {
    if (log_file == NULL) {
        return;
    }

    write_current_timestamp(log_file);

    fprintf(log_file, "\n");

    fflush(log_file);
}

void log_error(const char *msg) {
    if (log_file == NULL) {
        return;
    }

    write_current_timestamp(log_file);

    fprintf(log_file, " [ERROR] %s\n", msg);

    fflush(log_file);
}

void log_close(void)
{
    if (log_file != NULL)
    {
        fclose(log_file);
        log_file = NULL;
    }
}