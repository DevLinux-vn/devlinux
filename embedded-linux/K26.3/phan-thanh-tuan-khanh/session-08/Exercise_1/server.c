#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

static int server_fd = -1;

static void cleanup(void)
{
    if (server_fd >= 0) {
        close(server_fd);
    }

    unlink(SOCKET_PATH);
}

static void handle_sigint(int sig)
{
    (void)sig;

    printf("\n[Daemon] Shutting down...\n");

    cleanup();
    exit(0);
}

static void get_cpu_info(char *buffer, size_t size)
{
    FILE *fp;
    float load1;

    fp = fopen("/proc/loadavg", "r");
    if (fp == NULL) {
        snprintf(buffer, size,
                 "ERROR: cannot read /proc/loadavg");
        return;
    }

    if (fscanf(fp, "%f", &load1) == 1) {
        snprintf(buffer, size,
                 "load_avg=%.2f",
                 load1);
    } else {
        snprintf(buffer, size,
                 "ERROR: parse loadavg");
    }

    fclose(fp);
}

static void get_mem_info(char *buffer, size_t size)
{
    FILE *fp;
    char line[128];

    long mem_total = -1;
    long mem_free = -1;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        snprintf(buffer, size,
                 "ERROR: cannot read /proc/meminfo");
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemTotal: %ld kB", &mem_total) == 1) {
            continue;
        }

        if (sscanf(line, "MemFree: %ld kB", &mem_free) == 1) {
            continue;
        }

        if (mem_total >= 0 && mem_free >= 0) {
            break;
        }
    }

    fclose(fp);

    snprintf(buffer,
             size,
             "mem_total=%ld kB mem_free=%ld kB",
             mem_total,
             mem_free);
}

int main(void)
{
    struct sockaddr_un addr;
    int client_fd;

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    ssize_t n;

    signal(SIGINT, handle_sigint);

    unlink(SOCKET_PATH);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
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
             sizeof(addr)) < 0) {
        perror("bind");
        cleanup();
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        cleanup();
        return 1;
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        printf("[Daemon] Client connected.\n");

        while (1) {
            memset(buffer, 0, sizeof(buffer));

            n = recv(client_fd,
                     buffer,
                     sizeof(buffer) - 1,
                     0);

            if (n < 0) {
                perror("recv");
                break;
            }

            if (n == 0) {
                printf("[Daemon] Client disconnected. Waiting for next client...\n");
                break;
            }

            buffer[n] = '\0';

            buffer[strcspn(buffer, "\r\n")] = '\0';

            printf("[Daemon] CMD: %s\n", buffer);

            if (strcmp(buffer, "cpu") == 0) {
                get_cpu_info(response, sizeof(response));
            } else if (strcmp(buffer, "mem") == 0) {
                get_mem_info(response, sizeof(response));
            } else {
                snprintf(response,
                         sizeof(response),
                         "ERROR: unknown command");
            }

            if (send(client_fd,
                     response,
                     strlen(response),
                     0) < 0) {
                perror("send");
                break;
            }
        }

        close(client_fd);
    }

    cleanup();

    return 0;
}