#include <time.h>
#include <stdio.h>
#include "logger.h"

// Ghi thời gian hiện tại vào file (không có \n để nội dung khác nối vào)
void log_timestamp(void) {
    FILE *file = fopen("app.log", "a");
    if (file) {
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char buffer[26];
        // Format: YYYY-MM-DD HH:MM:SS
        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        fprintf(file, "[%s] ", buffer);
        fclose(file);
    }
}
void log_write(const char *msg) {
    FILE *file = fopen("app.log", "a");
    if (file) {
        fprintf(file, "%s\n", msg);
        fclose(file);
    }
}
void log_error(const char *msg) {
    FILE *file = fopen("app.log", "a");
    if (file) {
        fprintf(file, "[ERROR] %s\n", msg);
        fclose(file);
    }
}