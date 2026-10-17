#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_PORT 9000
#define BUFFER_SIZE    128

int main(void)
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(COLLECTOR_PORT);

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Collector] Listening on 0.0.0.0:%d...\n", COLLECTOR_PORT);
    fflush(stdout);

    char buffer[BUFFER_SIZE];

    for (;;) {
        client_len = sizeof(client_addr);
        memset(buffer, 0, sizeof(buffer));

        ssize_t n = recvfrom(sock_fd, buffer, sizeof(buffer) - 1, 0,
                              (struct sockaddr *)&client_addr, &client_len);
        if (n == -1) {
            perror("recvfrom");
            continue;
        }
        buffer[n] = '\0';

        /* Timestamp */
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char ts[16];
        strftime(ts, sizeof(ts), "%H:%M:%S", t);

        /* Sender address */
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));
        int port = ntohs(client_addr.sin_port);

        printf("[%s] %s:%d \xE2\x86\x92 %s\n", ts, ip_str, port, buffer);
        fflush(stdout);
    }

    close(sock_fd);
    return 0;
}