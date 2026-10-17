/*
    | Property                       | `flock`                   | `fcntl`                           |
    |------------------------------- |-------------------------- |---------------------------------- |
    | Lock granularity               | Whole file only           | Byte range supported              |
    | Works over NFS                 | No                        | Yes                               |
    | Inherited across `fork`        | Yes                       | No                                |
    | Best used when                 | Simple local file locking | Network FS or byte-range locking  | 
*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#include <sys/file.h>


#define LOG_FILE_NAME "system.log"
#define STR_FORMAT_SIZE 25
#define LOG_LEVEL LL_INFO

typedef enum {
    LL_NONE = 0,
    LL_ERROR,
    LL_WARNING,
    LL_INFO,
    LL_DEBUG,
}e_log_level_t;

static int get_log_time(char *str, int buffer_size);
static int write_log_msg(char *str, int byte_to_write);
int log_msg(char *str, e_log_level_t log_level);
static const char * log_level_to_string(e_log_level_t ll);

int main(int argc, char *argv[])
{
    (void)argc;
    if(log_msg(argv[1], LL_INFO) != 0) {
        printf("Error: Fail to log message to file %s.\n", LOG_FILE_NAME);
    }
    return 0;
}

int log_msg(char *str, e_log_level_t log_level)
{
    char buf_time[STR_FORMAT_SIZE] = {0};
    char buf_log[512] = {0};
    if(log_level > LOG_LEVEL){
        log_level = LOG_LEVEL;
    }
    if(get_log_time(buf_time, sizeof(buf_time)) != 0) {
        printf("Error: Fail to get time.\n");
        return 1;
    }
    int expected_write_len = snprintf(buf_log, sizeof(buf_log), "[PID:%d] [%s] [%s] %s\n", getpid(), buf_time, log_level_to_string(log_level), str);
    if(expected_write_len > 0) {
        if(write_log_msg(buf_log, expected_write_len) != 0) {
            perror("Error: Fail to write data.\n");
            return 1;
        }
    }
    return 0;
}

static int get_log_time(char *str, int buffer_size)
{
    time_t current_time = time(NULL);
    char buf[STR_FORMAT_SIZE];
    if(buffer_size < STR_FORMAT_SIZE) {
        printf("Error: Buffer size is smaller than expected.\n");
        return 1;
    }
    if (current_time == ((time_t)-1)) {
        printf("Error: Failure to obtain the current time.\n");
        return 1;
    }
    struct tm *t = localtime(&current_time);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    strncpy(str, (const char*)buf, buffer_size);
    return 0;
}

static int write_log_msg(char *str, int byte_to_write)
{
    int fd = open(LOG_FILE_NAME, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }

    if(flock(fd, LOCK_EX) == -1){
        perror("Error: Cannot lock file to write\n");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }
    ssize_t count = write(fd, (void*)str, strlen(str));
    if(count < 0) {
        perror("Error: Fail to write data\n");
        if(flock(fd, LOCK_UN) == -1) {
            perror("Error: Cannot unlock file\n");
            if(close(fd) == -1) {
                perror("Error: Cannot close file\n");
                return 1;
            }
            return 1;
        }
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;

    }
    else if(count != (long)byte_to_write) {
        printf("Error: Write data mismatch, expected:%d, actual:%lu\n", byte_to_write, count);
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }
    if(flock(fd, LOCK_UN) == -1) {
        perror("Error: Cannot unlock file\n");
        if(close(fd) == -1) {
            perror("Error: Cannot close file\n");
            return 1;
        }
        return 1;
    }
    if(close(fd) == -1) {
        perror("Error: Cannot close file\n");
        return 1;
    }
    return 0;
}

static const char * log_level_to_string(e_log_level_t ll)
{
    switch(ll){
        case LL_NONE: return "";
        case LL_ERROR: return "ERROR";
        case LL_WARNING: return "WARNING";
        case LL_INFO: return "INFO";
        case LL_DEBUG: return "DEBUG";
        default: return "";
    }
    return "";
}

