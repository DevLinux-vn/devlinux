#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

void trim_newline(char *s) {
    s[strcspn(s, "\r\n")] = '\0';
}

int main(void) {
    int client_fd;
    struct sockaddr_un addr;

    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    while (1) {
        char buffer[BUFFER_SIZE];
        char response[BUFFER_SIZE];

        printf("> ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        trim_newline(buffer);

        if (strcmp(buffer, "quit") == 0) {
            break;
        }

        if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
            perror("send");
            break;
        }

        ssize_t bytes_received = recv(client_fd, response, sizeof(response) - 1, 0);
        if (bytes_received == -1) {
            perror("recv");
            break;
        }

        if (bytes_received == 0) {
            printf("[monitor-cli] Daemon closed connection.\n");
            break;
        }

        response[bytes_received] = '\0';
        printf("%s\n", response);
    }

    close(client_fd);
    return 0;
}