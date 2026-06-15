#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <string.h>
#include <time.h>

#define LOG_FILE   "system.log"
#define LOG_LEVEL  "INFO"
#define BUFFER_SIZE 256


static int write_all(int fd, const char *buf, size_t len)
{
    size_t total = 0;

    while (total < len)
    {
        ssize_t ret = write(fd, buf + total, len - total);

        if (ret == -1)
        {
            return -1;
        }

        total += ret;
    }

    return 0;
}


int main(int argc, char *argv[])
{
    char log_buffer[BUFFER_SIZE];
    char time_buffer[32];

    time_t now;
    struct tm *tm_info;

    int fd;
    int length;


    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s \"message\"\n", argv[0]);
        return EXIT_FAILURE;
    }


    fd = open(LOG_FILE,
              O_WRONLY | O_APPEND | O_CREAT,
              0644);

    if (fd == -1)
    {
        perror("open");
        return EXIT_FAILURE;
    }


    if (flock(fd, LOCK_EX) == -1)
    {
        perror("flock");
        close(fd);
        return EXIT_FAILURE;
    }


    now = time(NULL);
    tm_info = localtime(&now);

    strftime(time_buffer,
             sizeof(time_buffer),
             "%Y-%m-%d %H:%M:%S",
             tm_info);


    length = snprintf(log_buffer,
                      sizeof(log_buffer),
                      "[PID:%d] [%s] [%s] %s\n",
                      getpid(),
                      time_buffer,
                      LOG_LEVEL,
                      argv[1]);


    if (write_all(fd, log_buffer, length) == -1)
    {
        perror("write");
    }


    if (flock(fd, LOCK_UN) == -1)
    {
        perror("flock unlock");
    }


    close(fd);

    return EXIT_SUCCESS;
}