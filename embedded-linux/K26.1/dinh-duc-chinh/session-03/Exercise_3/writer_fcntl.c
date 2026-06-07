/* (same comparison table comment as above) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"message\"\n", argv[0]);
        return 1;
    }

    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) { perror("open"); return 1; }

    struct flock fl = {
        .l_type   = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start  = 0,
        .l_len    = 0,
    };
    if (fcntl(fd, F_SETLKW, &fl) < 0) {
        perror("fcntl F_SETLKW");
        close(fd); return 1;
    }

    char ts[32];
    time_t now = time(NULL);
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&now));

    char line[512];
    int len = snprintf(line, sizeof(line),
                       "[PID:%d] [%s] [INFO] %s\n",
                       getpid(), ts, argv[1]);

    if (write(fd, line, len) != len) perror("write");

    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);
    close(fd);
    return 0;
}
