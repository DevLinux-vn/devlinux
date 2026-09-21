#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* --- Constants --- */
#define BIND_PORT           9000
#define MAX_BUFFER_SIZE     256
#define TIMESTAMP_SIZE      16

/* Global flag for graceful shutdown */
volatile sig_atomic_t g_is_running = 1;

/* --- Signal Handler --- */
static void handle_sigint(int signum) {
    (void)signum;
    g_is_running = 0;
}

/* --- Helper: Get Current Timestamp --- */
static void get_timestamp(char *buffer, size_t max_len) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t != NULL) {
        strftime(buffer, max_len, "%H:%M:%S", t);
    } else {
        strncpy(buffer, "00:00:00", max_len - 1);
        buffer[max_len - 1] = '\0';
    }
}

int main(void) {
    /* 1. Register SIGINT handler */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    /* 2. Create UDP Socket */
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) {
        perror("[Collector] socket failed");
        exit(EXIT_FAILURE);
    }

    /* 3. Enable SO_REUSEADDR */
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[Collector] setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* 4. Bind to 0.0.0.0:9000 */
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(BIND_PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[Collector] bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Collector] Listening on 0.0.0.0:%d...\n", BIND_PORT);

    /* 5. Receive Loop */
    char buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (g_is_running) {
        ssize_t bytes_received = recvfrom(server_fd, buffer, sizeof(buffer) - 1, 0,
                                          (struct sockaddr *)&client_addr, &client_len);
        
        if (bytes_received < 0) {
            if (errno == EINTR) {
                /* Graceful shutdown requested via SIGINT */
                break;
            }
            perror("[Collector] recvfrom failed");
            continue;
        }

        /* Null-terminate the string */
        buffer[bytes_received] = '\0';

        /* Strip trailing newlines if any */
        buffer[strcspn(buffer, "\r\n")] = '\0';

        /* Get Timestamp */
        char ts[TIMESTAMP_SIZE];
        get_timestamp(ts, sizeof(ts));

        /* Extract Client IP dynamically (inet_ntop is modern & thread-safe) */
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);

        /* Print result */
        printf("[%s] %s:%d → %s\n", ts, client_ip, client_port, buffer);
    }

    printf("\n[Collector] Shutting down...\n");
    close(server_fd);
    return EXIT_SUCCESS;
}
