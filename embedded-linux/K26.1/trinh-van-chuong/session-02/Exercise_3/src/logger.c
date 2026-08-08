#include "logger.h"
#include <stdio.h>
#include <time.h>

// Hàm nội bộ để lấy mốc thời gian định dạng YYYY-MM-DD HH:MM:SS
static void get_timestamp(char *buffer, size_t max_size) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, max_size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

void log_info(const char *message) {
    FILE *f = fopen("app.log", "a");
    if (!f) return;
    char timestamp[20];
    get_timestamp(timestamp, sizeof(timestamp));
    fprintf(f, "[%s] [INFO] %s\n", timestamp, message);
    fclose(f);
}

void log_error(const char *message) {
    FILE *f = fopen("app.log", "a");
    if (!f) return;
    char timestamp[20];
    get_timestamp(timestamp, sizeof(timestamp));
    fprintf(f, "[%s] [ERROR] %s\n", timestamp, message);
    fclose(f);
}