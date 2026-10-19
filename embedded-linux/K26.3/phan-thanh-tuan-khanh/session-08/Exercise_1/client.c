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
    int sockfd;
    struct sockaddr_un addr;

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    ssize_t n;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path,
            SOCKET_PATH,
            sizeof(addr.sun_path) - 1);

    if (connect(sockfd,
                (struct sockaddr *)&addr,
                sizeof(addr)) < 0)
    {
        perror("connect");
        close(sockfd);
        return 1;
    }

    printf("[monitor-cli] Connected to %s\n",
           SOCKET_PATH);

    while (1)
    {
        printf("> ");

        if (fgets(buffer,
                  sizeof(buffer),
                  stdin) == NULL)
        {
            break;
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "quit") == 0)
        {
            break;
        }

        if (send(sockfd,
                 buffer,
                 strlen(buffer),
                 0) < 0)
        {
            perror("send");
            break;
        }

        n = recv(sockfd,
                 response,
                 sizeof(response) - 1,
                 0);

        if (n < 0)
        {
            perror("recv");
            break;
        }

        if (n == 0)
        {
            printf("Server disconnected\n");
            break;
        }

        response[n] = '\0';

        printf("%s\n", response);
    }

    close(sockfd);

    return 0;
}