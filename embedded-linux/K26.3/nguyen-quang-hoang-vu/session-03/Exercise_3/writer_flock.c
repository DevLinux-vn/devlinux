/*
| Property | flock | fcntl |
|-----------|-----------|-----------|
| Lock granularity | Whole file only | Byte range supported |
| Works over NFS | No | Yes |
| Inherited across fork | Yes | No |
| Best used when | Simple local file locking | Network FS or byte-range locking |
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr,
                "Usage: %s \"message\"\n",
                argv[0]);
        return 1;
    }

    int fd;

    fd = open("system.log",
              O_WRONLY | O_APPEND | O_CREAT,
              0644);

    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    if (flock(fd, LOCK_EX) == -1)
    {
        perror("flock");
        close(fd);
        return 1;
    }

    time_t now = time(NULL);

    struct tm *tm_info =
        localtime(&now);

    if (tm_info == NULL)
    {
        fprintf(stderr,
                "localtime failed\n");

        close(fd);
        return 1;
    }

    char timebuf[64];

    strftime(timebuf,
             sizeof(timebuf),
             "%Y-%m-%d %H:%M:%S",
             tm_info);

    char buffer[512];

    snprintf(buffer,
             sizeof(buffer),
             "[PID:%d] [%s] [INFO] %s\n",
             getpid(),
             timebuf,
             argv[1]);

    if (write(fd,
              buffer,
              strlen(buffer))
        < 0)
    {
        perror("write");
    }

    flock(fd, LOCK_UN);

    close(fd);

    return 0;
}

