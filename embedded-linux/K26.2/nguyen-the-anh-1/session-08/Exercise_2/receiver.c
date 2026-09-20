#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define COLLECTOR_PORT 9000
#define BUFFER_SIZE 256

int sockfd;

void handle_sigint(int sig)
{
    (void)sig;

    printf("\n[Collector] Shutting down...\n");

    if (close(sockfd) == -1) {
        perror("close");
    }

    exit(0);
}

int main(void)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    char buffer[BUFFER_SIZE];

    socklen_t client_addr_len;

    /* Register SIGINT handler */
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    /* Create UDP socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    /* Enable SO_REUSEADDR */
    int reuse = 1;

    if (setsockopt(sockfd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuse,
                   sizeof(reuse)) == -1) {

        perror("setsockopt");
        close(sockfd);
        return 1;
    }

    /* Configure server address */
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(COLLECTOR_PORT);

    /* Bind */
    if (bind(sockfd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {

        perror("bind");
        close(sockfd);
        return 1;
    }

    printf("[Collector] Listening on 0.0.0.0:%d...\n",
           COLLECTOR_PORT);

    while (1) {

        client_addr_len = sizeof(client_addr);

        int bytes_received = recvfrom(
            sockfd,
            buffer,
            sizeof(buffer) - 1,
            0,
            (struct sockaddr *)&client_addr,
            &client_addr_len
        );

        if (bytes_received == -1) {

            if (errno == EINTR) {
                continue;
            }

            perror("recvfrom");
            break;
        }

        buffer[bytes_received] = '\0';

        /* Get current time */
        time_t now = time(NULL);

        if (now == (time_t)-1) {
            perror("time");
            continue;
        }

        struct tm *current_time = localtime(&now);

        if (current_time == NULL) {
            perror("localtime");
            continue;
        }

        char timestamp[16];

        if (strftime(timestamp,
                     sizeof(timestamp),
                     "%H:%M:%S",
                     current_time) == 0) {

            fprintf(stderr, "strftime failed\n");
            continue;
        }

        /* Convert sender IP to string */
        char sender_ip[INET_ADDRSTRLEN];

        if (inet_ntop(AF_INET,
                      &client_addr.sin_addr,
                      sender_ip,
                      sizeof(sender_ip)) == NULL) {

            perror("inet_ntop");
            continue;
        }

        /* Get sender port */
        int sender_port = ntohs(client_addr.sin_port);

        printf("[%s] %s:%d → %s\n",
               timestamp,
               sender_ip,
               sender_port,
               buffer);

        fflush(stdout);
    }

    if (close(sockfd) == -1) {
        perror("close");
    }

    return 0;
}
