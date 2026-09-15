#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int server_fd = -1;

void handle_sigint(int sig)
{
    (void)sig;

    if (server_fd != -1)
    {
        close(server_fd);
    }

    unlink(SOCKET_PATH);

    printf("\n[Daemon] Shutting down...\n");

    exit(0);
}

void handle_cpu(int client_fd)
{
    FILE *file;
    char line[BUFFER_SIZE];
    char load_avg[64];

    file = fopen("/proc/loadavg", "r");

    if (file == NULL)
    {
        const char *error = "ERROR: cannot read /proc/loadavg";
        send(client_fd, error, strlen(error), 0);
        return;
    }

    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);

        const char *error = "ERROR: cannot read load average";
        send(client_fd, error, strlen(error), 0);
        return;
    }

    fclose(file);

    if (sscanf(line, "%63s", load_avg) != 1)
    {
        const char *error = "ERROR: invalid load average";
        send(client_fd, error, strlen(error), 0);
        return;
    }

    char response[BUFFER_SIZE];

    snprintf(response, sizeof(response),
             "load_avg=%s",
             load_avg);

    if (send(client_fd, response, strlen(response), 0) == -1)
    {
        perror("send");
    }
}

void handle_mem(int client_fd)
{
    FILE *file;
    char line[BUFFER_SIZE];

    long mem_total = -1;
    long mem_free = -1;

    file = fopen("/proc/meminfo", "r");

    if (file == NULL)
    {
        const char *error = "ERROR: cannot read /proc/meminfo";
        send(client_fd, error, strlen(error), 0);
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "MemTotal: %ld kB", &mem_total) == 1)
        {
            continue;
        }

        if (sscanf(line, "MemFree: %ld kB", &mem_free) == 1)
        {
            continue;
        }

        if (mem_total != -1 && mem_free != -1)
        {
            break;
        }
    }

    fclose(file);

    if (mem_total == -1 || mem_free == -1)
    {
        const char *error = "ERROR: cannot read memory information";
        send(client_fd, error, strlen(error), 0);
        return;
    }

    char response[BUFFER_SIZE];

    snprintf(response, sizeof(response),
             "mem_total=%ld kB mem_free=%ld kB",
             mem_total,
             mem_free);

    if (send(client_fd, response, strlen(response), 0) == -1)
    {
        perror("send");
    }
}

int main(void)
{
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    signal(SIGINT, handle_sigint);

    unlink(SOCKET_PATH);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (server_fd == -1)
    {
        perror("socket");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path,
            SOCKET_PATH,
            sizeof(addr.sun_path) - 1);

    if (bind(server_fd,
             (struct sockaddr *)&addr,
             sizeof(addr)) == -1)
    {
        perror("bind");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }

    if (listen(server_fd, 5) == -1)
    {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }

    setbuf(stdout, NULL);

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    while (1)
    {
        int client_fd;

        client_fd = accept(server_fd, NULL, NULL);

        if (client_fd == -1)
        {
            perror("accept");
            continue;
        }

        printf("[Daemon] Client connected.\n");

        while (1)
        {
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
                printf("[Daemon] Client disconnected. "
                       "Waiting for next client...\n");
                break;
            }

            buffer[received] = '\0';

            buffer[strcspn(buffer, "\r\n")] = '\0';

            printf("[Daemon] CMD: %s\n", buffer);

            if (strcmp(buffer, "cpu") == 0)
            {
                handle_cpu(client_fd);
            }
            else if (strcmp(buffer, "mem") == 0)
            {
                handle_mem(client_fd);
            }
            else
            {
                const char *error = "ERROR: unknown command";

                if (send(client_fd,
                         error,
                         strlen(error),
                         0) == -1)
                {
                    perror("send");
                }
            }
        }

        close(client_fd);
    }

    return 0;
}
