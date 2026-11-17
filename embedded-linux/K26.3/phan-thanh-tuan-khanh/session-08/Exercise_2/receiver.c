#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_PORT 9000
#define BUFFER_SIZE 256

int main(void)
{
    int sockfd;
    int reuse = 1;

    struct sockaddr_in server_addr;
    struct sockaddr_in sender_addr;

    socklen_t sender_len;
    ssize_t bytes_received;

    char buffer[BUFFER_SIZE];
    char timestamp[16];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    if (setsockopt(sockfd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuse,
                   sizeof(reuse)) < 0)
    {
        perror("setsockopt");
        close(sockfd);
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(COLLECTOR_PORT);

    if (bind(sockfd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(sockfd);
        return 1;
    }

    printf("[Collector] Listening on 0.0.0.0:%d...\n",
           COLLECTOR_PORT);

    while (1)
    {
        time_t now;
        struct tm *tm_info;

        sender_len = sizeof(sender_addr);

        bytes_received = recvfrom(sockfd,
                                  buffer,
                                  sizeof(buffer) - 1,
                                  0,
                                  (struct sockaddr *)&sender_addr,
                                  &sender_len);

        if (bytes_received < 0)
        {
            perror("recvfrom");
            continue;
        }

        buffer[bytes_received] = '\0';

        now = time(NULL);
        tm_info = localtime(&now);

        if (tm_info == NULL)
        {
            strcpy(timestamp, "00:00:00");
        }
        else
        {
            strftime(timestamp,
                     sizeof(timestamp),
                     "%H:%M:%S",
                     tm_info);
        }

        printf("[%s] %s:%d -> %s\n",
               timestamp,
               inet_ntoa(sender_addr.sin_addr),
               ntohs(sender_addr.sin_port),
               buffer);
    }

    close(sockfd);

    return 0;
}