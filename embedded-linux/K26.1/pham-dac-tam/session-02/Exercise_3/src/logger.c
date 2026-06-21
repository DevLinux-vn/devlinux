#include <stdio.h>
#include <time.h>
#include "logger.h"

#define LOG_FILE "app.log"

void log_write(const char *message) {
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) return;
    
    fprintf(file, "%s\n", message);
    fclose(file);
}

void log_timestamp(void) {
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) return;
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char timestamp[50];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(file, "[%s]\n", timestamp);
    fclose(file);
}

void log_error(const char *message) {
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) return;
    
    fprintf(file, "[ERROR] %s\n", message);
    fclose(file);
}
