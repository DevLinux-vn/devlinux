/* Copyright (C) 2025 XYZ Corp. All rights reserved. */

/*
 * Comparison: flock vs fcntl
 *
 * | Property | flock | fcntl |
 * |---|---|---|
 * | Lock granularity | Whole file only | Byte range supported |
 * | Works over NFS | No | Yes |
 * | Inherited across fork | Yes | No |
 * | Best used when | Simple local file locking | Network FS or byte-range locking |
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <time.h>

void format_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return 1;
    }

    int fd = open("system.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    if (flock(fd, LOCK_EX) == -1) {
        perror("Error acquiring lock");
        close(fd);
        return 1;
    }

    char timestamp[64];
    format_timestamp(timestamp, sizeof(timestamp));

    char log_line[512];
    snprintf(log_line, sizeof(log_line), "[PID:%d] [%s] [INFO] %s\n",
             getpid(), timestamp, argv[1]);

    if (write(fd, log_line, strlen(log_line)) == -1) {
        perror("Error writing to file");
        flock(fd, LOCK_UN);
        close(fd);
        return 1;
    }

    if (flock(fd, LOCK_UN) == -1) {
        perror("Error releasing lock");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
