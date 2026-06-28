#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int main(void) {
    setbuf(stdout, NULL);

    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("CRITICAL: Socket creation failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    if (connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("CRITICAL: Connect to daemon failed");
        close(client_fd);
        return EXIT_FAILURE;
    }

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    char cmd_buf[BUFFER_SIZE];
    char res_buf[BUFFER_SIZE];

    while (1) {
        printf("> ");
        if (fgets(cmd_buf, sizeof(cmd_buf), stdin) == NULL) {
            break;
        }

        cmd_buf[strcspn(cmd_buf, "\r\n")] = '\0';

        if (strlen(cmd_buf) == 0) {
            continue;
        }

        if (strcmp(cmd_buf, "quit") == 0) {
            break;
        }

        if (send(client_fd, cmd_buf, strlen(cmd_buf), 0) < 0) {
            perror("ERROR: Send failed");
            break;
        }

        memset(res_buf, 0, sizeof(res_buf));
        ssize_t bytes_read = recv(client_fd, res_buf, sizeof(res_buf) - 1, 0);
        if (bytes_read < 0) {
            perror("ERROR: Recv failed");
            break;
        } else if (bytes_read == 0) {
            printf("ERROR: Server closed connection.\n");
            break;
        }

        printf("%s\n", res_buf);
    }

    close(client_fd);
    return EXIT_SUCCESS;
}