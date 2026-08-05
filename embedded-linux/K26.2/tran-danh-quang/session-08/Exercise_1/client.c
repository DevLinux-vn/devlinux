#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

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
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", SOCKET_PATH);

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

        size_t total_sent = 0;
        size_t input_len = strlen(input);
        while (total_sent < input_len) {
            ssize_t sent_bytes = send(client_fd, input + total_sent, input_len - total_sent, 0);
            if (sent_bytes == -1) {
                if (errno == EINTR) {
                    continue;
                }
                perror("send");
                break;
            }
            total_sent += sent_bytes;
        }
        
        if (total_sent < input_len) {
            break;
        }

        ssize_t total_read = 0;
        while (total_read < (ssize_t)(sizeof(response) - 1)) {
            ssize_t bytes_read = recv(client_fd, response + total_read, sizeof(response) - 1 - total_read, 0);
            if (bytes_read == -1) {
                if (errno == EINTR) {
                    continue;
                }
                perror("recv");
                break;
            } else if (bytes_read == 0) {
                break;
            }
            
            total_read += bytes_read;
            if (response[total_read - 1] == '\n') {
                break;
            }
        }
        
        if (total_read == 0) {
            printf("[monitor-cli] Server disconnected.\n");
            break;
        }

        response[total_read] = '\0';
        printf("%s", response);
        if (total_read > 0 && response[total_read - 1] != '\n') {
            printf("\n");
        }
    }

    close(client_fd);
    return 0;
}
