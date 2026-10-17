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
#include <sys/file.h>
#include <time.h>

#define LOG_FILE "system.log"
#define BUFFER_SIZE 512

void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

int main(int argc, char *argv[]) {
    int fd;
    char timestamp[64];
    char log_line[BUFFER_SIZE];

    if (argc < 2) {
        write(STDERR_FILENO, "Usage: ./writer_flock \"message text\"\n", 38);
        return 1;
    }

    fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (flock(fd, LOCK_EX) == -1) {
        perror("flock LOCK_EX");
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
        flock(fd, LOCK_UN);
        close(fd);
        return 1;
    }

    if (flock(fd, LOCK_UN) == -1) {
        perror("flock LOCK_UN");
        close(fd);
        return 1;
    }

    close(fd);

    return 0;
}