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

void cleanup_and_exit(int signum) {
    (void)signum;

    if (server_fd != -1) {
        close(server_fd);
    }

    unlink(SOCKET_PATH);
    printf("\n[Daemon] Socket removed. Exiting...\n");
    exit(0);
}

void trim_newline(char *s) {
    s[strcspn(s, "\r\n")] = '\0';
}

void handle_cpu(char *response, size_t size) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (f == NULL) {
        snprintf(response, size, "ERROR: cannot read /proc/loadavg");
        return;
    }

    double load1;
    if (fscanf(f, "%lf", &load1) != 1) {
        snprintf(response, size, "ERROR: invalid /proc/loadavg format");
        fclose(f);
        return;
    }

    fclose(f);
    snprintf(response, size, "load_avg=%.2f", load1);
}

void handle_mem(char *response, size_t size) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (f == NULL) {
        snprintf(response, size, "ERROR: cannot read /proc/meminfo");
        return;
    }

    char line[128];
    long mem_total = -1;
    long mem_free = -1;

    while (fgets(line, sizeof(line), f) != NULL) {
        if (sscanf(line, "MemTotal: %ld kB", &mem_total) == 1) {
            continue;
        }

        if (sscanf(line, "MemFree: %ld kB", &mem_free) == 1) {
            continue;
        }

        if (mem_total != -1 && mem_free != -1) {
            break;
        }
    }

    fclose(f);

    if (mem_total == -1 || mem_free == -1) {
        snprintf(response, size, "ERROR: cannot parse /proc/meminfo");
        return;
    }

    snprintf(response, size, "mem_total=%ld kB mem_free=%ld kB", mem_total, mem_free);
}

int main(void) {
    struct sockaddr_un addr;

    signal(SIGINT, cleanup_and_exit);

    unlink(SOCKET_PATH);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        printf("[Daemon] Client connected.\n");

        while (1) {
            char buffer[BUFFER_SIZE];
            char response[BUFFER_SIZE];

            ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received == -1) {
                perror("recv");
                break;
            }

            if (bytes_received == 0) {
                printf("[Daemon] Client disconnected. Waiting for next client...\n");
                break;
            }

            buffer[bytes_received] = '\0';
            trim_newline(buffer);

            printf("[Daemon] CMD: %s\n", buffer);

            if (strcmp(buffer, "cpu") == 0) {
                handle_cpu(response, sizeof(response));
            } else if (strcmp(buffer, "mem") == 0) {
                handle_mem(response, sizeof(response));
            } else {
                snprintf(response, sizeof(response), "ERROR: unknown command");
            }

            if (send(client_fd, response, strlen(response), 0) == -1) {
                perror("send");
                break;
            }
        }

        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}