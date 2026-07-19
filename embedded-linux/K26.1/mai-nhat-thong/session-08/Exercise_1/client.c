/**
 * @file client.c
 * @brief CLI Tool to query the System Monitor Daemon.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int main(void) {
    int sock_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    /* 1. Create Unix Domain Socket */
    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    /* 2. Connect to the Daemon */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed. Is the daemon running?");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    /* 3. Interactive Loop */
    while (1) {
        printf("> ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        /* Clean up trailing newline */
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "quit") == 0) {
            break;
        }

        /* Prevent sending empty strings */
        if (strlen(buffer) == 0) {
            continue;
        }

        /* Send command to daemon */
        if (send(sock_fd, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            break;
        }

        /* Receive response */
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            printf("[monitor-cli] Server closed connection.\n");
            break;
        }

        printf("%s", buffer);
    }

    close(sock_fd);
    return EXIT_SUCCESS;
}