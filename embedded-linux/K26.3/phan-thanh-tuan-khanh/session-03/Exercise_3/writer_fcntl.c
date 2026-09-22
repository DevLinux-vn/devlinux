/*
Comparison

flock:
- Lock granularity: Whole file only
- Works over NFS: No
- Inherited across fork: Yes
- Best used when: Simple local file locking

fcntl:
- Lock granularity: Byte range supported
- Works over NFS: Yes
- Inherited across fork: No
- Best used when: Network FS or byte-range locking
*/

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
        fprintf(stderr,
                "Usage: %s \"message\"\n",
                argv[0]);
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

    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    if (fcntl(fd, F_SETLKW, &fl) == -1)
    {
        perror("fcntl lock");
        close(fd);
        return 1;
    }

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    char timebuf[32];

    strftime(timebuf,
             sizeof(timebuf),
             "%Y-%m-%d %H:%M:%S",
             tm_info);

    char logline[512];

    snprintf(logline,
             sizeof(logline),
             "[PID:%d] [%s] [INFO] %s\n",
             getpid(),
             timebuf,
             argv[1]);

    ssize_t n = write(fd,
                      logline,
                      strlen(logline));

    if (n < 0)
    {
        perror("write");

        fl.l_type = F_UNLCK;

        if (fcntl(fd, F_SETLK, &fl) == -1)
        {
            perror("fcntl unlock");
        }

        close(fd);
        return 1;
    }

    fl.l_type = F_UNLCK;

    if (fcntl(fd, F_SETLK, &fl) == -1)
    {
        perror("fcntl unlock");
    }

    if (close(fd) == -1)
    {
        perror("close");
        return 1;
    }

    return 0;
}