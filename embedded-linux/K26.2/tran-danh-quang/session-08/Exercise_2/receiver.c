#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#define COLLECTOR_PORT 9000
#define BUFFER_SIZE 512

volatile sig_atomic_t keep_running = 1;

static void sigint_handler(int signum) {
    (void)signum;
    keep_running = 0;
}

int main(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(server_fd);
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    server_addr.sin_port = htons(COLLECTOR_PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    printf("[Collector] Listening on 0.0.0.0:%d...\n", COLLECTOR_PORT);

    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (keep_running) {
        ssize_t bytes_read = recvfrom(server_fd, buffer, sizeof(buffer) - 1, 0,
                                      (struct sockaddr *)&client_addr, &client_len);
        
        if (bytes_read == -1) {
            // Check if we were interrupted by a signal
            if (errno == EINTR) {
                continue;
            }
            perror("recvfrom");
            continue;
        }

        buffer[bytes_read] = '\0';
        
        // Remove trailing newline if any
        buffer[strcspn(buffer, "\r\n")] = '\0';

        time_t now = time(NULL);
        if (now == (time_t)-1) {
            perror("time");
            continue;
        }
        
        struct tm *t = localtime(&now);
        if (!t) {
            perror("localtime");
            continue;
        }

        char ts[16];
        if (strftime(ts, sizeof(ts), "%H:%M:%S", t) == 0) {
            fprintf(stderr, "strftime failed\n");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN) == NULL) {
            snprintf(client_ip, sizeof(client_ip), "UNKNOWN");
        }
        
        int client_port = ntohs(client_addr.sin_port);

        printf("[%s] %s:%d → %s\n", ts, client_ip, client_port, buffer);
    }

    printf("\n[Collector] Shutting down...\n");
    close(server_fd);
    return 0;
}
