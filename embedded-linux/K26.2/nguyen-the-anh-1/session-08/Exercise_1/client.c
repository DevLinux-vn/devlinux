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

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    /* Create socket */
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (client_fd == -1) {
        perror("socket");
        return 1;
    }

    /* Configure address */
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path,
            SOCKET_PATH,
            sizeof(addr.sun_path) - 1);

    /* Connect */
    if (connect(client_fd,
                (struct sockaddr *)&addr,
                sizeof(addr)) == -1) {

        perror("connect");
        close(client_fd);
        return 1;
    }

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    while (1) {

        printf("> ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        /* Remove newline */
        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (strcmp(buffer, "quit") == 0) {
            break;
        }

        /* Send command */
        size_t length = strlen(buffer);

        if (send(client_fd,
                 buffer,
                 length,
                 0) == -1) {

            perror("send");
            break;
        }

        /* Receive response */
        int bytes_received;

        bytes_received = recv(client_fd,
                              response,
                              sizeof(response) - 1,
                              0);

        if (bytes_received == -1) {
            perror("recv");
            break;
        }

        if (bytes_received == 0) {
            printf("Daemon disconnected.\n");
            break;
        }

        response[bytes_received] = '\0';

        printf("%s", response);
    }

    close(client_fd);

    return 0;
}
