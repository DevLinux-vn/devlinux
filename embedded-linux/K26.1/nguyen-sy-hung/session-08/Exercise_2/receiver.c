#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define LISTEN_IP              "0.0.0.0"
#define LISTEN_PORT            9000U

#define BUFFER_SIZE            256U
#define TIMESTAMP_SIZE         16U

#define SOCKET_BACKLOG         1U

static int g_socket_fd = -1;

/*------------------------------------------------------------------*/

static void cleanup(void)
{
    if (g_socket_fd >= 0)
    {
        if (close(g_socket_fd) == -1)
        {
            perror("close");
        }

        g_socket_fd = -1;
    }
}

/*------------------------------------------------------------------*/

static void signal_handler(int signo)
{
    (void)signo;

    printf("\n[Collector] Shutting down...\n");

    cleanup();

    exit(EXIT_SUCCESS);
}

/*------------------------------------------------------------------*/

static int create_receiver_socket(void)
{
    int socket_fd;
    int reuse_addr = 1;

    struct sockaddr_in server_addr;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1)
    {
        perror("socket");
        return -1;
    }

    if (setsockopt(socket_fd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuse_addr,
                   sizeof(reuse_addr)) == -1)
    {
        perror("setsockopt");
        close(socket_fd);
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(LISTEN_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(socket_fd);
        return -1;
    }

    return socket_fd;
}

/*------------------------------------------------------------------*/

static int get_timestamp(char *timestamp, size_t size)
{
    time_t now;
    struct tm *local_time;

    now = time(NULL);
    if (now == (time_t)-1)
    {
        perror("time");
        return -1;
    }

    local_time = localtime(&now);
    if (local_time == NULL)
    {
        perror("localtime");
        return -1;
    }

    if (strftime(timestamp,
                 size,
                 "%H:%M:%S",
                 local_time) == 0U)
    {
        fprintf(stderr, "strftime failed\n");
        return -1;
    }

    return 0;
}

/*------------------------------------------------------------------*/

static void receive_loop(int socket_fd)
{
    char buffer[BUFFER_SIZE];
    char timestamp[TIMESTAMP_SIZE];
    char ip_address[INET_ADDRSTRLEN];

    struct sockaddr_in sender_addr;
    socklen_t sender_len;

    ssize_t received_bytes;

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        sender_len = sizeof(sender_addr);

        received_bytes = recvfrom(socket_fd,
                                  buffer,
                                  sizeof(buffer) - 1,
                                  0,
                                  (struct sockaddr *)&sender_addr,
                                  &sender_len);

        if (received_bytes == -1)
        {
            perror("recvfrom");
            continue;
        }

        buffer[received_bytes] = '\0';

        if (get_timestamp(timestamp, sizeof(timestamp)) == -1)
        {
            continue;
        }

        if (inet_ntop(AF_INET,
                      &sender_addr.sin_addr,
                      ip_address,
                      sizeof(ip_address)) == NULL)
        {
            perror("inet_ntop");
            continue;
        }

        printf("[%s] %s:%u -> %s\n",
               timestamp,
               ip_address,
               ntohs(sender_addr.sin_port),
               buffer);
    }
}

/*------------------------------------------------------------------*/

int main(void)
{
    signal(SIGINT, signal_handler);

    g_socket_fd = create_receiver_socket();
    if (g_socket_fd == -1)
    {
        return EXIT_FAILURE;
    }

    printf("[Collector] Listening on %s:%u...\n",
           LISTEN_IP,
           LISTEN_PORT);

    receive_loop(g_socket_fd);

    cleanup();

    return EXIT_SUCCESS;
}