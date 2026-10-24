#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

/*
| Property | flock | fcntl |
|---|---|---|
| Lock granularity | Whole file only | Byte range supported |
| Works over NFS | No | Yes |
| Inherited across fork | Yes | No |
| Best used when | Simple local file locking | Network FS or byte-range locking |
*/

int main(int argc, char *argv[])
{
    if (argc < 2) 
    {
        printf("[ERROR] Miss argument\n");
        return 1;
    }

    int fd = open("system.log", O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1) {
        printf("[ERROR] Cannot open file\n");
        return 1;
    }

    struct flock fl = {
        .l_type   = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start  = 0,
        .l_len    = 0,
    };

    char message[100];
    int pid = (int)getpid();
    time_t now;

    time(&now);
    struct tm *local_time = localtime(&now);

    snprintf(message, sizeof(message),
                "[PID:%d] [%04d-%02d-%02d %02d:%02d:%02d] [INFO] %s\n",
                pid,
                local_time->tm_year + 1900,
                local_time->tm_mon + 1,
                local_time->tm_mday,
                local_time->tm_hour,
                local_time->tm_min,
                local_time->tm_sec,
                argv[1]);

    fcntl(fd, F_SETLKW, &fl);
    ssize_t bytes = write(fd, message, strlen(message));
    if (bytes == -1) {
        printf("[ERROR] Cannot write file\n");

        fl.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &fl);

        close(fd);
        return 1;
    }

    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);

    close(fd);
    return 0;
}