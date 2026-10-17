#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int main() {
    // Create Unix Domain Socket
    int client_fd = socket(AF_UNIX,  SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(1);
    }

    // Configure address
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Connect
    if (connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(client_fd);
        exit(1);
    }   
    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    char input[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove trailing newline
        size_t len = strlen(input);
        if (input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        if (strcmp(input, "quit") == 0) {
            break;
        }

        // Send command to daemon
        if (send(client_fd, input, strlen(input), 0) == -1) {
            perror("send");
            break;
        }

        // Receive response
        memset(response, 0, BUFFER_SIZE);
        ssize_t n = recv(client_fd, response, sizeof(response) - 1, 0);
        if (n == -1) {
            perror("recv");
            break;
        }

        if (n == 0) {
            printf("[monitor-cli] Daemon closed the connection.\n");
            break;
        }
        
        response[n] = '\0';
        printf("%s\n", response);
    }

    close(client_fd);
    return 0;
}