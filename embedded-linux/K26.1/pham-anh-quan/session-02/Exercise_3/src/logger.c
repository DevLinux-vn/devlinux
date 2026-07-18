#include "logger.h"
#include <stdio.h>
#include <time.h>

// Hàm ghi log có kèm timestamp vào file app.log và in ra màn hình
void log_write(const char *msg) {
    FILE *f = fopen("app.log", "a");
    if (!f) return;

    time_t rawtime;
    struct tm *timeinfo;
    char buffer[20];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Ghi vào file
    fprintf(f, "[%s] %s\n", buffer, msg);
    fclose(f);

    // In song song ra terminal cho dễ theo dõi
    printf("[%s] %s\n", buffer, msg);
}

void log_timestamp(void) {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[20];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    printf("%s\n", buffer);
}

void log_error(const char *msg) {
    char error_msg[256];
    snprintf(error_msg, sizeof(error_msg), "[ERROR] %s", msg);
    log_write(error_msg);
}