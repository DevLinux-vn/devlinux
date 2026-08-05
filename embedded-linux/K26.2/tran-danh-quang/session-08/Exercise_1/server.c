#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

volatile sig_atomic_t keep_running = 1;

static void sigint_handler(int signum) {
    (void)signum;
    keep_running = 0;
}

static int read_cpu_load(char *buffer, size_t size) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) {
        perror("fopen /proc/loadavg");
        return -1;
    }
    float load1;
    if (fscanf(f, "%f", &load1) != 1) {
        fclose(f);
        return -1;
    }
    fclose(f);
    snprintf(buffer, size, "load_avg=%.2f\n", load1);
    return 0;
}

static int read_mem_info(char *buffer, size_t size) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        perror("fopen /proc/meminfo");
        return -1;
    }
    char line[128];
    long mem_total = -1, mem_free = -1;
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld kB", &mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %ld kB", &mem_free);
        }
    }
    fclose(f);
    if (mem_total == -1 || mem_free == -1) {
        return -1;
    }
    snprintf(buffer, size, "mem_total=%ld kB mem_free=%ld kB\n", mem_total, mem_free);
    return 0;
}

static int setup_server_socket(void) {
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", SOCKET_PATH);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

int main(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    int server_fd = setup_server_socket();
    if (server_fd == -1) {
        return 1;
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    while (keep_running) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            continue;
        }

        printf("[Daemon] Client connected.\n");

        char buffer[BUFFER_SIZE];
        while (keep_running) {
            ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_read == -1) {
                if (errno == EINTR) {
                    continue;
                }
                perror("recv");
                break;
            } else if (bytes_read == 0) {
                printf("[Daemon] Client disconnected. Waiting for next client...\n");
                break;
            }

            buffer[bytes_read] = '\0';
            
            // Remove trailing newline if any
            buffer[strcspn(buffer, "\r\n")] = '\0';

            if (strlen(buffer) == 0) {
                continue;
            }

            printf("[Daemon] CMD: %s\n", buffer);

            char response[BUFFER_SIZE];
            if (strcmp(buffer, "cpu") == 0) {
                if (read_cpu_load(response, sizeof(response)) == -1) {
                    snprintf(response, sizeof(response), "ERROR: failed to read cpu load\n");
                }
            } else if (strcmp(buffer, "mem") == 0) {
                if (read_mem_info(response, sizeof(response)) == -1) {
                    snprintf(response, sizeof(response), "ERROR: failed to read mem info\n");
                }
            } else {
                snprintf(response, sizeof(response), "ERROR: unknown command\n");
            }

            size_t total_sent = 0;
            size_t response_len = strlen(response);
            while (total_sent < response_len) {
                ssize_t sent_bytes = send(client_fd, response + total_sent, response_len - total_sent, 0);
                if (sent_bytes == -1) {
                    if (errno == EINTR) {
                        continue;
                    }
                    perror("send");
                    break;
                }
                total_sent += sent_bytes;
            }
        }
        close(client_fd);
    }

    printf("\n[Daemon] Shutting down...\n");
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
