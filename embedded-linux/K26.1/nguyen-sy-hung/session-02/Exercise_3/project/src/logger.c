#include "logger.h" 
#include <stdio.h>
#include <time.h>

#define LOG_FILE "app.log"

static void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", info);
}

void logger_print_timestamp(void) {
    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));
    printf("%s\n", timestamp);
}

void logger_log(const char *message) {
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        return;
    }
    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));
    fprintf(file, "[%s] %s\n", timestamp, message);
    fclose(file);
}

void logger_error(const char *message) {
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        return;
    }
    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));
    fprintf(file, "[%s] [ERROR] %s\n", timestamp, message);
    fclose(file);
}