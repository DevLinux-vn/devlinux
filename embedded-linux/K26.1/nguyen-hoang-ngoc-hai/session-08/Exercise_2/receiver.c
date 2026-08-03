#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_PORT 9000
#define BUFFER_SIZE    128

static int sock_fd = -1;

void handle_sigint(int signum) {
    (void)signum;

    if (sock_fd != -1) {
        close(sock_fd);
    }

    printf("\n[Collector] Stopped.\n");
    exit(0);
}

void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    if (t == NULL) {
        snprintf(buffer, size, "00:00:00");
        return;
    }

    strftime(buffer, size, "%H:%M:%S", t);
}

int main(void) {
    struct sockaddr_in server_addr;

    signal(SIGINT, handle_sigint);

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
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

    while (1) {
        char buffer[BUFFER_SIZE];
        char timestamp[16];
        char sender_ip[INET_ADDRSTRLEN];

        struct sockaddr_in sender_addr;
        socklen_t sender_len = sizeof(sender_addr);

        ssize_t received_bytes = recvfrom(
            sock_fd,
            buffer,
            sizeof(buffer) - 1,
            0,
            (struct sockaddr *)&sender_addr,
            &sender_len
        );

        if (received_bytes == -1) {
            perror("recvfrom");
            continue;
        }

        buffer[received_bytes] = '\0';

        if (inet_ntop(
                AF_INET,
                &sender_addr.sin_addr,
                sender_ip,
                sizeof(sender_ip)
            ) == NULL) {
            perror("inet_ntop");
            continue;
        }

        get_timestamp(timestamp, sizeof(timestamp));

        printf(
            "[%s] %s:%d -> %s\n",
            timestamp,
            sender_ip,
            ntohs(sender_addr.sin_port),
            buffer
        );
    }

    close(sock_fd);
    return 0;
}