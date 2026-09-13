#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define COLLECTOR_PORT 9000
#define BUFFER_SIZE 128

static int sockfd = -1;
static volatile sig_atomic_t running = 1;

static void handle_sigint(int sig)
{
    (void)sig;
    running = 0;
}

int main(void)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in sender_addr;

    char buffer[BUFFER_SIZE];

    socklen_t sender_addr_len;

    /*
     * Install SIGINT handler
     *
     * Ctrl+C -> running = 0
     */
    if (signal(SIGINT, handle_sigint) == SIG_ERR)
    {
        perror("signal");
        return EXIT_FAILURE;
    }

    /*
     * Create UDP socket
     *
     * AF_INET    -> IPv4
     * SOCK_DGRAM -> UDP
     */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    /*
     * Allow socket to reuse local address
     */
    int reuse = 1;

    if (setsockopt(sockfd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuse,
                   sizeof(reuse)) < 0)
    {
        perror("setsockopt");
        close(sockfd);
        return EXIT_FAILURE;
    }

    /*
     * Configure server address
     */
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;

    /*
     * INADDR_ANY
     *
     * Receive UDP packets sent to port 9000
     * on any local IPv4 interface.
     */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    server_addr.sin_port = htons(COLLECTOR_PORT);

    /*
     * Bind socket to 0.0.0.0:9000
     */
    if (bind(sockfd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("[Collector] Listening on 0.0.0.0:%d...\n",
           COLLECTOR_PORT);

    while (running)
    {
        time_t now;
        struct tm *local_time;

        char timestamp[16];

        memset(buffer, 0, sizeof(buffer));
        memset(&sender_addr, 0, sizeof(sender_addr));

        sender_addr_len = sizeof(sender_addr);

        /*
         * Wait for one UDP datagram
         */
        ssize_t received_bytes = recvfrom(
            sockfd,
            buffer,
            sizeof(buffer) - 1,
            0,
            (struct sockaddr *)&sender_addr,
            &sender_addr_len
        );

        if (received_bytes < 0)
        {
            if (errno == EINTR)
            {
                /*
                 * recvfrom() was interrupted by Ctrl+C
                 */
                continue;
            }

            perror("recvfrom");
            close(sockfd);
            return EXIT_FAILURE;
        }

        /*
         * Make sure buffer is NULL-terminated
         */
        buffer[received_bytes] = '\0';

        /*
         * Get current time
         */
        now = time(NULL);

        if (now == (time_t)-1)
        {
            perror("time");
            continue;
        }

        local_time = localtime(&now);

        if (local_time == NULL)
        {
            perror("localtime");
            continue;
        }

        if (strftime(timestamp,
                     sizeof(timestamp),
                     "%H:%M:%S",
                     local_time) == 0)
        {
            fprintf(stderr, "strftime failed\n");
            continue;
        }

        /*
         * Convert sender IP to printable string
         */
        char sender_ip[INET_ADDRSTRLEN];

        if (inet_ntop(AF_INET,
                      &sender_addr.sin_addr,
                      sender_ip,
                      sizeof(sender_ip)) == NULL)
        {
            perror("inet_ntop");
            continue;
        }

        /*
         * Print:
         *
         * [HH:MM:SS] IP:port -> message
         */
        printf("[%s] %s:%u -> %s\n",
               timestamp,
               sender_ip,
               ntohs(sender_addr.sin_port),
               buffer);
    }

    printf("[Collector] Shutting down...\n");

    if (close(sockfd) < 0)
    {
        perror("close");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
