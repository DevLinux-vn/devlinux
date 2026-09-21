#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int fd = open("system.log", O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1) {
        perror("[ERROR] Cannot open file");
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

    while (1)
    {
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

        fl.l_type = F_WRLCK;
        fcntl(fd, F_SETLKW, &fl);
        ssize_t bytes = write(fd, message, strlen(message));
        if (bytes == -1) {
            perror("[ERROR] Cannot write file");

            fl.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &fl);

            close(fd);
            return 1;
        }

        sleep(1);
        fl.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &fl);
    }

    close(fd);
    return 0;
}