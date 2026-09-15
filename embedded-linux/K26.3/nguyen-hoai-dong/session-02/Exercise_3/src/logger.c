#include <stdio.h>
#include <time.h>

void log_write(const char *msg)
{
    FILE *fp;
    fp = fopen("app.log", "a");
    if (fp == NULL)
    {
        printf("Cannot open file\n");
    }
    else
    {
        fprintf(fp, "%s\n", msg);
    }
    fclose(fp);
}

void log_timestamp(void)
{
    time_t now;
    struct tm *local_time;
    FILE *fp;
    
    fp = fopen("app.log", "a");
    time(&now);
    local_time = localtime(&now);

    if (fp == NULL)
    {
        printf("Cannot open file\n");
    }
    else
    {
        fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d\n",
                local_time->tm_year + 1900,
                local_time->tm_mon + 1,
                local_time->tm_mday,
                local_time->tm_hour,
                local_time->tm_min,
                local_time->tm_sec);
    }
    fclose(fp);
}

void log_error(const char *msg)
{
    FILE *fp;
    fp = fopen("app.log", "a");
    if (fp == NULL)
    {
        printf("Cannot open file\n");
    }
    else
    {
        fprintf(fp, "[ERROR] %s\n", msg);
    }
    fclose(fp);
}