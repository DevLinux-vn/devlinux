#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define COLLECTOR_PORT 9000
#define BUFFER_SIZE 128

int collector_fd = -1;

void handle_sigint(int sig)
{
    (void)sig;

    printf("\n[Collector] Shutting down...\n");

    if (collector_fd != -1)
    {
        close(collector_fd);
    }

    exit(0);
}

int main(void)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    signal(SIGINT, handle_sigint);

    collector_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (collector_fd == -1)
    {
        perror("socket");
        return 1;
    }

    int reuse = 1;

    if (setsockopt(collector_fd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuse,
                   sizeof(reuse)) == -1)
    {
        perror("setsockopt");
        close(collector_fd);
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(COLLECTOR_PORT);

    if (bind(collector_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(collector_fd);
        return 1;
    }

    setbuf(stdout, NULL);

    printf("[Collector] Listening on 0.0.0.0:%d...\n",
           COLLECTOR_PORT);

    while (1)
    {
        ssize_t received;

        memset(buffer, 0, sizeof(buffer));

        client_len = sizeof(client_addr);

        received = recvfrom(collector_fd,
                            buffer,
                            sizeof(buffer) - 1,
                            0,
                            (struct sockaddr *)&client_addr,
                            &client_len);

        if (received == -1)
        {
            perror("recvfrom");
            continue;
        }

        buffer[received] = '\0';

        time_t now = time(NULL);
        struct tm *current_time = localtime(&now);

        if (current_time == NULL)
        {
            perror("localtime");
            continue;
        }

        char timestamp[16];

        if (strftime(timestamp,
                     sizeof(timestamp),
                     "%H:%M:%S",
                     current_time) == 0)
        {
            fprintf(stderr, "strftime failed\n");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];

        if (inet_ntop(AF_INET,
                      &client_addr.sin_addr,
                      client_ip,
                      sizeof(client_ip)) == NULL)
        {
            perror("inet_ntop");
            continue;
        }

        printf("[%s] %s:%d -> %s\n",
               timestamp,
               client_ip,
               ntohs(client_addr.sin_port),
               buffer);
    }

    return 0;
}
