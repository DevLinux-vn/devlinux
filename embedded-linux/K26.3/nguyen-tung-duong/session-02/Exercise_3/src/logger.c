#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_LOG_LEN 512
#define LOG_FILE "app.log"

/* Write log message to app.log */
void log_write(const char *msg) {
    if (msg == (void *)0) return;
    
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        printf("Error: Cannot open log file\n");
        return;
    }
    
    fprintf(file, "%s\n", msg);
    fclose(file);
}

/* Write timestamp to log */
void log_timestamp(void) {
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        printf("Error: Cannot open log file\n");
        return;
    }
    
    fprintf(file, "[%s]\n", timestamp);
    fclose(file);
}

/* Write error message with [ERROR] prefix */
void log_error(const char *msg) {
    if (msg == (void *)0) return;
    
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        printf("Error: Cannot open log file\n");
        return;
    }
    
    fprintf(file, "[ERROR] %s\n", msg);
    fclose(file);
}
