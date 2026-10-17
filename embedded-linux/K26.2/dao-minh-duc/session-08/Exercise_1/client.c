#define _POSIX_C_SOURCE 200809L

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

    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (client_fd == -1)
    {
        perror("socket");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path,
            SOCKET_PATH,
            sizeof(addr.sun_path) - 1);

    if (connect(client_fd,
                 (struct sockaddr *)&addr,
                 sizeof(addr)) == -1)
    {
        perror("connect");
        close(client_fd);
        return 1;
    }

    setbuf(stdout, NULL);

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    while (1)
    {
        printf("> ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            break;
        }

        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (strcmp(buffer, "quit") == 0)
        {
            break;
        }

        if (strlen(buffer) == 0)
        {
            continue;
        }

        if (send(client_fd,
                 buffer,
                 strlen(buffer),
                 0) == -1)
        {
            perror("send");
            break;
        }

        ssize_t received;

        memset(buffer, 0, sizeof(buffer));

        received = recv(client_fd,
                        buffer,
                        sizeof(buffer) - 1,
                        0);

        if (received == -1)
        {
            perror("recv");
            break;
        }

        if (received == 0)
        {
            printf("Server disconnected.\n");
            break;
        }

        buffer[received] = '\0';

        printf("%s\n", buffer);
    }

    close(client_fd);

    return 0;
}
