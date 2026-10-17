#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s \"message\"\n", argv[0]);
        return 1;
    }

    int fd = open(LOG_FILE,
                  O_WRONLY | O_APPEND | O_CREAT,
                  0644);

    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl lock");
        close(fd);
        return 1;
    }

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    char timestamp[32];

    strftime(timestamp,
             sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S",
             tm_info);

    char buffer[512];

    snprintf(buffer,
             sizeof(buffer),
             "[PID:%d] [%s] [INFO] %s\n",
             getpid(),
             timestamp,
             argv[1]);

    write(fd, buffer, strlen(buffer));

    lock.l_type = F_UNLCK;

    if (fcntl(fd, F_SETLK, &lock) == -1)
    {
        perror("fcntl unlock");
    }

    close(fd);

    return 0;
}