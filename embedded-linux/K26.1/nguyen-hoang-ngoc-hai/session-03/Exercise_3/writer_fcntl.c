/*
Comparison table:

| Property | flock | fcntl |
|---|---|---|
| Lock granularity | Whole file only | Byte range supported |
| Works over NFS | No | Yes |
| Inherited across fork | Yes | No |
| Best used when | Simple local file locking | Network FS or byte-range locking |
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define LOG_FILE "system.log"
#define BUFFER_SIZE 512

void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return 1;
    }

    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct flock fl;

    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    if (fcntl(fd, F_SETLKW, &fl) < 0) {
        perror("fcntl F_SETLKW");
        close(fd);
        return 1;
    }

    char timestamp[20];
    char line[BUFFER_SIZE];

    get_timestamp(timestamp, sizeof(timestamp));

    int len = snprintf(
        line,
        sizeof(line),
        "[PID:%d] [%s] [INFO] %s\n",
        getpid(),
        timestamp,
        argv[1]
    );

    if (len < 0 || len >= (int)sizeof(line)) {
        fprintf(stderr, "Log message too long.\n");
        fl.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &fl);
        close(fd);
        return 1;
    }

    if (write(fd, line, len) != len) {
        perror("write");
        fl.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &fl);
        close(fd);
        return 1;
    }

    fl.l_type = F_UNLCK;

    if (fcntl(fd, F_SETLK, &fl) < 0) {
        perror("fcntl F_UNLCK");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}