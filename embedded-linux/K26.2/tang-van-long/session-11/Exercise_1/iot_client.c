#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "iot_server.h"

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

static void print_server_line(const char *line)
{
    const char *tag = "[SERVER_STATUS]";
    if (strncmp(line, tag, strlen(tag)) == 0)
        printf("[BROADCAST]%s\n", line + strlen(tag));
    else
        printf("%s\n", line);
    fflush(stdout);
}

int main(void)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return EXIT_FAILURE;
    }

    char sbuf[512]; size_t slen = 0;     
    char ibuf[512]; size_t ilen = 0;     
    int stdin_open = 1;

    for (;;) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
        int maxfd = sock;
        if (stdin_open) {
            FD_SET(STDIN_FILENO, &rfds);
            if (STDIN_FILENO > maxfd)
                maxfd = STDIN_FILENO;
        }

        if (select(maxfd + 1, &rfds, NULL, NULL, NULL) < 0) {
            if (errno == EINTR)
                continue;
            perror("select");
            break;
        }

        if (FD_ISSET(sock, &rfds)) {
            ssize_t n = recv(sock, sbuf + slen, sizeof(sbuf) - 1 - slen, 0);
            if (n < 0) {
                if (errno == EINTR)
                    continue;
                perror("recv");
                break;
            }
            if (n == 0) {
                if (slen > 0) { sbuf[slen] = '\0'; print_server_line(sbuf); }
                break;                
            }
            slen += (size_t)n;
            char *start = sbuf, *nl;
            while ((nl = memchr(start, '\n', sbuf + slen - start)) != NULL) {
                *nl = '\0';
                print_server_line(start);
                start = nl + 1;
            }
            slen = (size_t)(sbuf + slen - start);
            memmove(sbuf, start, slen);
            if (slen >= sizeof(sbuf) - 1) slen = 0;
        }

        if (stdin_open && FD_ISSET(STDIN_FILENO, &rfds)) {
            ssize_t n = read(STDIN_FILENO, ibuf + ilen, sizeof(ibuf) - 1 - ilen);
            if (n < 0) {
                if (errno == EINTR)
                    continue;
                perror("read");
                break;
            }
            if (n == 0) {             
                stdin_open = 0;
                send_all(sock, "QUIT\n", 5);
                shutdown(sock, SHUT_WR);
                continue;
            }
            ilen += (size_t)n;
            char *start = ibuf, *nl;
            int quit = 0;
            while (!quit && (nl = memchr(start, '\n', ibuf + ilen - start)) != NULL) {
                *nl = '\0';
                char *cmd = start;
                size_t l = strlen(cmd);
                while (l > 0 && (cmd[l - 1] == '\r' || cmd[l - 1] == ' '))
                    cmd[--l] = '\0';
                start = nl + 1;
                if (l == 0)
                    continue;
                if (strcasecmp(cmd, "quit") == 0) {
                    send_all(sock, "QUIT\n", 5);
                    quit = 1;
                } else {
                    char out[520];
                    int m = snprintf(out, sizeof(out), "%s\n", cmd);
                    if (send_all(sock, out, (size_t)m) < 0)
                        quit = 1;
                }
            }
            if (quit)
                break;
            ilen = (size_t)(ibuf + ilen - start);
            memmove(ibuf, start, ilen);
            if (ilen >= sizeof(ibuf) - 1) ilen = 0;
        }
    }

    close(sock);
    return EXIT_SUCCESS;
}