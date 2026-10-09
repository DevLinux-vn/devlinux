#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int main() {
    // 1. socket(AF_UNIX, SOCK_STREAM, 0)
    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // 2. connect()
    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    char cmd_buf[BUFFER_SIZE];
    char resp_buf[BUFFER_SIZE];

    // 3. Loop
    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(cmd_buf, sizeof(cmd_buf), stdin) == NULL) {
            break;
        }

        // Trim newline characters
        cmd_buf[strcspn(cmd_buf, "\r\n")] = 0;

        // Check if "quit" -> break
        if (strcmp(cmd_buf, "quit") == 0) {
            break;
        }

        if (strlen(cmd_buf) == 0) {
            continue;
        }

        // Send command to daemon
        if (send(sock_fd, cmd_buf, strlen(cmd_buf), 0) == -1) {
            perror("send failed");
            break;
        }

        // Receive result
        memset(resp_buf, 0, sizeof(resp_buf));
        ssize_t n = recv(sock_fd, resp_buf, sizeof(resp_buf) - 1, 0);
        if (n < 0) {
            perror("recv failed");
            break;
        } else if (n == 0) {
            printf("Server closed connection.\n");
            break;
        }

        // Print to stdout
        printf("%s", resp_buf);
    }

    // 4. close()
    close(sock_fd);
    return 0;
}