#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int main(void)
{
    int client_fd;
    struct sockaddr_un addr;

    /*
     * Create Unix Domain Stream Socket.
     */
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (client_fd < 0) {
        perror("[monitor-cli] socket");
        return EXIT_FAILURE;
    }

    /*
     * Configure socket address.
     */
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path,
            SOCKET_PATH,
            sizeof(addr.sun_path) - 1);

    /*
     * Connect to daemon.
     */
    if (connect(client_fd,
                (struct sockaddr *)&addr,
                sizeof(addr)) < 0) {
        perror("[monitor-cli] connect");
        close(client_fd);
        return EXIT_FAILURE;
    }

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    while (1) {
        char command[BUFFER_SIZE];
        char response[BUFFER_SIZE];

        /*
         * Read command from stdin.
         */
        printf("> ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        /*
         * Remove newline.
         */
        command[strcspn(command, "\r\n")] = '\0';

        /*
         * quit is handled locally.
         */
        if (strcmp(command, "quit") == 0) {
            break;
        }

        /*
         * Send command to daemon.
         */
        if (send(client_fd,
                 command,
                 strlen(command),
                 0) < 0) {
            perror("[monitor-cli] send");
            break;
        }

        /*
         * Receive daemon response.
         */
        memset(response, 0, sizeof(response));

        ssize_t bytes_received = recv(client_fd,
                                      response,
                                      sizeof(response) - 1,
                                      0);

        if (bytes_received < 0) {
            perror("[monitor-cli] recv");
            break;
        }

        if (bytes_received == 0) {
            printf("[monitor-cli] Server disconnected.\n");
            break;
        }

        response[bytes_received] = '\0';

        printf("%s", response);
    }

    if (close(client_fd) < 0) {
        perror("[monitor-cli] close");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
