#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

/* --- Constants (No Magic Numbers) --- */
#define SOCKET_PATH         "/tmp/monitor.sock"
#define MAX_BUFFER_SIZE     256
#define CMD_QUIT            "quit"

int main(void) {
    /* 1. Create Socket */
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("[monitor-cli] socket failed");
        exit(EXIT_FAILURE);
    }

    /* 2. Configure Address & Connect */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("[monitor-cli] connect failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    /* 3. Command Loop */
    char input[MAX_BUFFER_SIZE];
    char response[MAX_BUFFER_SIZE];

    while (1) {
        printf("> ");
        fflush(stdout);

        /* Read from stdin */
        if (!fgets(input, sizeof(input), stdin)) {
            break; /* EOF or error */
        }

        /* Strip trailing newline */
        input[strcspn(input, "\r\n")] = '\0';

        /* Ignore empty inputs */
        if (strlen(input) == 0) {
            continue;
        }

        /* Handle local "quit" command */
        if (strcmp(input, CMD_QUIT) == 0) {
            break;
        }

        /* Send command to Daemon */
        ssize_t bytes_sent = send(client_fd, input, strlen(input), 0);
        if (bytes_sent < 0) {
            perror("[monitor-cli] send failed");
            break;
        }

        /* Receive response from Daemon */
        ssize_t bytes_read = recv(client_fd, response, sizeof(response) - 1, 0);
        if (bytes_read < 0) {
            perror("[monitor-cli] recv failed");
            break;
        } else if (bytes_read == 0) {
            printf("[monitor-cli] Daemon closed the connection.\n");
            break;
        }

        /* Null-terminate and print */
        response[bytes_read] = '\0';
        printf("%s\n", response);
    }

    /* 4. Cleanup */
    close(client_fd);
    return EXIT_SUCCESS;
}