#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "event_monitor.h"

static int send_all(int fd, const char *buf, size_t len)
{
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, MSG_NOSIGNAL);
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
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return EXIT_FAILURE;
    }

    char line[LINE_BUF_SIZE];
    char resp[LINE_BUF_SIZE];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r' ||
                           line[len - 1] == ' '))
            line[--len] = '\0';
        if (len == 0)
            continue;

        line[len++] = '\n';             
        line[len] = '\0';
        if (send_all(sock, line, len) < 0)
            break;

        if (strncasecmp(line, "exit", 4) == 0 && len == 5)
            break;                     

        ssize_t n = recv(sock, resp, sizeof(resp) - 1, 0);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            perror("recv");
            break;
        }
        if (n == 0) {
            fprintf(stderr, "Server closed the connection\n");
            break;
        }
        resp[n] = '\0';
        fputs(resp, stdout);
        fflush(stdout);
    }

    close(sock);                       
    return EXIT_SUCCESS;
}