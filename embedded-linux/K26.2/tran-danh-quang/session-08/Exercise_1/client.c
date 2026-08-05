#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int main(void) {
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(client_fd);
        return 1;
    }

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    char input[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        input[strcspn(input, "\r\n")] = '\0';

        if (strcmp(input, "quit") == 0) {
            break;
        }

        if (strlen(input) == 0) {
            continue;
        }

        if (send(client_fd, input, strlen(input), 0) == -1) {
            perror("send");
            break;
        }

        ssize_t bytes_read = recv(client_fd, response, sizeof(response) - 1, 0);
        if (bytes_read == -1) {
            perror("recv");
            break;
        } else if (bytes_read == 0) {
            printf("[monitor-cli] Server disconnected.\n");
            break;
        }

        response[bytes_read] = '\0';
        printf("%s", response);
        if (response[bytes_read - 1] != '\n') {
            printf("\n");
        }
    }

    close(client_fd);
    return 0;
}
