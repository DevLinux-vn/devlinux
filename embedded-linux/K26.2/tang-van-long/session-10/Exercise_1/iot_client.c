#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "iot_server.h"

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
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
    addr.sin_port = htons(SERVER_PORT);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("[Client] Connected to %s:%d\n", SERVER_HOST, SERVER_PORT);

    char line[BUF_SIZE];
    char recvbuf[BUF_SIZE];

    for (;;) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);
        int max_fd = (sock > STDIN_FILENO) ? sock : STDIN_FILENO;

        int ready = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (ready < 0) {
            if (errno == EINTR)
                continue;
            perror("select");
            break;
        }

        if (FD_ISSET(sock, &readfds)) {
            ssize_t n = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
            if (n <= 0) {
                printf("[Client] Server closed connection\n");
                break;
            }
            recvbuf[n] = '\0';
            char *saveptr = NULL;
            char *tok = strtok_r(recvbuf, "\n", &saveptr);
            while (tok != NULL) {
                if (strncmp(tok, "[SERVER_STATUS]", 15) == 0)
                    printf("[BROADCAST] %s\n", tok + 16);
                else
                    printf("%s\n", tok);
                tok = strtok_r(NULL, "\n", &saveptr);
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (fgets(line, sizeof(line), stdin) == NULL) {
                printf("[Client] EOF, closing.\n");
                break;
            }
            size_t len = strlen(line);
            if (len == 0 || line[len - 1] != '\n') {
                line[len] = '\n';
                line[len + 1] = '\0';
                len++;
            }

            if (send_all(sock, line, len) < 0)
                break;
            char trimmed[BUF_SIZE];
            strncpy(trimmed, line, sizeof(trimmed) - 1);
            trimmed[sizeof(trimmed) - 1] = '\0';
            trimmed[strcspn(trimmed, "\r\n")] = '\0';
            if (strcmp(trimmed, "QUIT") == 0 || strcmp(trimmed, "quit") == 0) {
                break;
            }
        }
    }

    close(sock);
    return 0;
}