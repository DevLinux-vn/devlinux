/*
Comparison Table

Property                    flock                      fcntl
---------------------------------------------------------------------------
Lock granularity             Whole file only            Byte range supported
Works over NFS               No                         Yes
Inherited across fork        Yes (child gets a copy)    No
Automatically released       Yes                        Yes
Best used when               Simple local locking       Network FS or byte-range locking
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
    struct tm *t = localtime(&now);

    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

int lock_file(int fd) {
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    return fcntl(fd, F_SETLKW, &fl);
}

int unlock_file(int fd) {
    struct flock fl;

    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    return fcntl(fd, F_SETLK, &fl);
}

int main(int argc, char *argv[]) {
    int fd;
    char timestamp[64];
    char log_line[BUFFER_SIZE];

    if (argc < 2) {
        write(STDERR_FILENO, "Usage: ./writer_fcntl \"message text\"\n", 38);
        return 1;
    }

    fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (lock_file(fd) == -1) {
        perror("fcntl F_SETLKW");
        close(fd);
        return 1;
    }

    get_timestamp(timestamp, sizeof(timestamp));

    snprintf(
        log_line,
        sizeof(log_line),
        "[PID:%d] [%s] [INFO] %s\n",
        getpid(),
        timestamp,
        argv[1]
    );

    if (write(fd, log_line, strlen(log_line)) == -1) {
        perror("write");
        unlock_file(fd);
        close(fd);
        return 1;
    }

    if (unlock_file(fd) == -1) {
        perror("fcntl F_UNLCK");
        close(fd);
        return 1;
    }

    close(fd);

    return 0;
}