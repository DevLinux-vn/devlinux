#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <time.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return 1;
    }

    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return 1;
    }

   
    if (flock(fd, LOCK_EX) == -1) {
        perror("flock");
        close(fd);
        return 1;
    }

   
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);

    char timestamp[32];

    strftime(timestamp,
             sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S",
             time_info);

    
    char log_line[1024];

    int length = snprintf(log_line,
                          sizeof(log_line),
                          "[PID:%d] [%s] [INFO] %s\n",
                          getpid(),
                          timestamp,
                          argv[1]);

  
    if (write(fd, log_line, length) == -1) {
        perror("write");
        flock(fd, LOCK_UN);
        close(fd);
        return 1;
    }

    if (flock(fd, LOCK_UN) == -1) {
        perror("flock unlock");
        close(fd);
        return 1;
    }

    close(fd);

    return 0;
}