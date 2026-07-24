#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH        "/tmp/monitor.sock"

#define BUFFER_SIZE        256U
#define LOADAVG_SIZE       64U
#define MEMINFO_LINE_SIZE  128U

#define LISTEN_BACKLOG     5

#define CMD_CPU            "cpu"
#define CMD_MEM            "mem"

#define UNKNOWN_COMMAND    "ERROR: unknown command"

static int g_server_fd = -1;

/*------------------------------------------------------------------*/

static void cleanup_socket(void)
{
    if (g_server_fd >= 0)
    {
        close(g_server_fd);
        g_server_fd = -1;
    }

    unlink(SOCKET_PATH);
}

/*------------------------------------------------------------------*/

static void signal_handler(int signo)
{
    (void)signo;

    printf("\n[Daemon] Shutting down...\n");

    cleanup_socket();

    exit(EXIT_SUCCESS);
}

/*------------------------------------------------------------------*/

static int create_server_socket(void)
{
    int server_fd;
    struct sockaddr_un server_addr;

    unlink(SOCKET_PATH);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;

    strncpy(server_addr.sun_path,
            SOCKET_PATH,
            sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(server_fd);
        unlink(SOCKET_PATH);
        return -1;
    }

    if (listen(server_fd, LISTEN_BACKLOG) == -1)
    {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return -1;
    }

    return server_fd;
}

/*------------------------------------------------------------------*/

static void read_cpu_info(char *response, size_t size)
{
    FILE *fp;
    char load_avg[LOADAVG_SIZE];

    fp = fopen("/proc/loadavg", "r");
    if (fp == NULL)
    {
        snprintf(response, size, "ERROR: cannot read /proc/loadavg");
        return;
    }

    if (fscanf(fp, "%63s", load_avg) != 1)
    {
        snprintf(response, size, "ERROR: invalid loadavg format");
        fclose(fp);
        return;
    }

    fclose(fp);

    snprintf(response, size, "load_avg=%s", load_avg);
}

/*------------------------------------------------------------------*/

static void read_mem_info(char *response, size_t size)
{
    FILE *fp;

    char line[MEMINFO_LINE_SIZE];

    long mem_total = 0;
    long mem_free = 0;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        snprintf(response, size, "ERROR: cannot read /proc/meminfo");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line, "MemTotal: %ld", &mem_total) == 1)
        {
            continue;
        }

        if (sscanf(line, "MemFree: %ld", &mem_free) == 1)
        {
            continue;
        }

        if ((mem_total != 0) && (mem_free != 0))
        {
            break;
        }
    }

    fclose(fp);

    snprintf(response,
             size,
             "mem_total=%ld kB mem_free=%ld kB",
             mem_total,
             mem_free);
}

/*------------------------------------------------------------------*/

static void process_command(const char *command,
                            char *response,
                            size_t response_size)
{
    if (strcmp(command, CMD_CPU) == 0)
    {
        read_cpu_info(response, response_size);
    }
    else if (strcmp(command, CMD_MEM) == 0)
    {
        read_mem_info(response, response_size);
    }
    else
    {
        snprintf(response,
                 response_size,
                 "%s",
                 UNKNOWN_COMMAND);
    }
}

/*------------------------------------------------------------------*/

static void handle_client(int client_fd)
{
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    ssize_t received_bytes;
    ssize_t sent_bytes;

    printf("[Daemon] Client connected.\n");

    while (1)
    {
        memset(command, 0, sizeof(command));
        memset(response, 0, sizeof(response));

        received_bytes = recv(client_fd,
                              command,
                              sizeof(command) - 1,
                              0);

        if (received_bytes == -1)
        {
            perror("recv");
            break;
        }

        if (received_bytes == 0)
        {
            printf("[Daemon] Client disconnected.\n");
            break;
        }

        command[strcspn(command, "\r\n")] = '\0';

        printf("[Daemon] CMD: %s\n", command);

        process_command(command,
                        response,
                        sizeof(response));

        sent_bytes = send(client_fd,
                          response,
                          strlen(response),
                          0);

        if (sent_bytes == -1)
        {
            perror("send");
            break;
        }
    }

    close(client_fd);
}

/*------------------------------------------------------------------*/

int main(void)
{
    int client_fd;

    signal(SIGINT, signal_handler);

    g_server_fd = create_server_socket();
    if (g_server_fd == -1)
    {
        return EXIT_FAILURE;
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    while (1)
    {
        client_fd = accept(g_server_fd, NULL, NULL);

        if (client_fd == -1)
        {
            perror("accept");
            continue;
        }

        handle_client(client_fd);
    }

    cleanup_socket();

    return EXIT_SUCCESS;
}