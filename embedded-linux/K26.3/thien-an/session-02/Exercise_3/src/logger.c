#include "logger.h"
#include <stdio.h>
#include <time.h>

/* include/logger.h */
void log_write(const char *msg)
{
    FILE *file = fopen("app.log", "a");
    if (file != NULL)
    {
        fprintf(file, "[INFO] %s\n", msg);
        fclose(file);
    }
}
void log_timestamp(void)
{
    FILE *file = fopen("app.log", "a");
    if (file != NULL)
    {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char time_str[20];

        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
        fprintf(file, "=== Timestamp : %s ===\n", time_str);
        fclose(file);
    }
}
void log_error(const char *msg)
{
    FILE *file = fopen("app.log", "a");
    if (file != NULL)
    {
        fprintf(file, "[ERROR] %s\n", msg);
        fclose(file);
    }
}