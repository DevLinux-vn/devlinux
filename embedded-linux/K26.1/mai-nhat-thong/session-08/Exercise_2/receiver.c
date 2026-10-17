
/**
 * @file receiver.c
 * @brief UDP Collector for Sensor Telemetry.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

#define BIND_PORT 9000
#define BUFFER_SIZE 256

static volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

int main(void) {
    int sock_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    /* Signal Handler for graceful shutdown */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    /* 1. Create UDP Socket */
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    /* 2. Enable SO_REUSEADDR to prevent "Address already in use" on rapid restarts */
    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    /* 3. Bind to 0.0.0.0:9000 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(BIND_PORT);

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Collector] Listening on 0.0.0.0:%d...\n", BIND_PORT);

    /* 4. Receive Loop */
    while (keep_running) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t n = recvfrom(sock_fd, buffer, BUFFER_SIZE - 1, 0, 
                            (struct sockaddr *)&client_addr, &client_len);
        
        if (n < 0) {
            if (!keep_running) break; /* Interrupted by SIGINT */
            perror("recvfrom failed");
            continue;
        }

        buffer[n] = '\0'; /* Null-terminate the received data */

        /* Generate timestamp */
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char ts[16];
        strftime(ts, sizeof(ts), "%H:%M:%S", t);

        /* Print telemetry data */
        printf("[%s] %s:%d → %s\n", 
               ts, 
               inet_ntoa(client_addr.sin_addr), 
               ntohs(client_addr.sin_port), 
               buffer);
    }

    printf("\n[Collector] Shutting down.\n");
    close(sock_fd);
    return EXIT_SUCCESS;
}