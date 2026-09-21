#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "event_monitor.h"

static int send_all(int fd, const char *buf, size_t len)
{
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            perror("send");
            return -1;
        }
        sent += (size_t)n;
    }
    return 0;
}

int main(void)
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        exit(EXIT_FAILURE);
    }

    char line[BUF_SIZE];
    char resp[BUF_SIZE];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0)
            continue;

        char upper[BUF_SIZE];
        strncpy(upper, line, sizeof(upper) - 1);
        upper[sizeof(upper) - 1] = '\0';
        for (char *p = upper; *p; p++)
            *p = (char)toupper((unsigned char)*p);

        char to_send[BUF_SIZE];
        snprintf(to_send, sizeof(to_send), "%s\n", line);
        if (send_all(fd, to_send, strlen(to_send)) < 0)
            break;

        if (strcmp(upper, "EXIT") == 0) {
            break;
        }

        ssize_t n = recv(fd, resp, sizeof(resp) - 1, 0);
        if (n <= 0) {
            printf("[Client] Server closed connection\n");
            break;
        }
        resp[n] = '\0';
        printf("%s", resp);
        if (resp[strlen(resp) - 1] != '\n')
            printf("\n");
    }

    close(fd);
    return 0;
}