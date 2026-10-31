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

void cleanup_socket() {
    if (server_fd != -1) {
        close(server_fd);
    }
    unlink(SOCKET_PATH);
}

void handle_sigint(int sig) {
    (void)sig;
    printf("\n[Daemon] Shutting down cleanly...\n");
    cleanup_socket();
    exit(0);
}

void handle_cpu_cmd(int client_fd) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) {
        char *err_msg = "ERROR: cannot read /proc/loadavg\n";
        send(client_fd, err_msg, strlen(err_msg), 0);
        return;
    }
    double load1;
    if (fscanf(f, "%lf", &load1) != 1) {
        load1 = 0.0;
    }
    fclose(f);

    char resp[BUFFER_SIZE];
    snprintf(resp, sizeof(resp), "load_avg=%.2lf\n", load1);
    send(client_fd, resp, strlen(resp), 0);
}

void handle_mem_cmd(int client_fd) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        char *err_msg = "ERROR: cannot read /proc/meminfo\n";
        send(client_fd, err_msg, strlen(err_msg), 0);
        return;
    }
    char line[128];
    long long mem_total = 0, mem_free = 0;
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "MemTotal: %lld kB", &mem_total) == 1) continue;
        if (sscanf(line, "MemFree: %lld kB", &mem_free) == 1) continue;
    }
    fclose(f);

    char resp[BUFFER_SIZE];
    snprintf(resp, sizeof(resp), "mem_total=%lld kB mem_free=%lld kB\n", mem_total, mem_free);
    send(client_fd, resp, strlen(resp), 0);
}

int main() {
    // 1. Register SIGINT handler to unlink socket file on exit
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // 2. Remove stale socket if it exists
    unlink(SOCKET_PATH);

    // 3. socket(AF_UNIX, SOCK_STREAM, 0)
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure Unix Domain Socket address
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // 4. bind() -> listen()
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind failed");
        cleanup_socket();
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen failed");
        cleanup_socket();
        exit(EXIT_FAILURE);
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    // 5. Client loop
    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            perror("accept failed");
            continue;
        }
        printf("[Daemon] Client connected.\n");

        char buffer[BUFFER_SIZE];
        // Command loop
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (n < 0) {
                perror("recv failed");
                break;
            } else if (n == 0) {
                printf("[Daemon] Client disconnected. Waiting for next client...\n");
                break; // client disconnected -> break inner loop
            }

            // Strip newline if present
            buffer[strcspn(buffer, "\r\n")] = 0;

            if (strcmp(buffer, "cpu") == 0) {
                printf("[Daemon] CMD: cpu\n");
                handle_cpu_cmd(client_fd);
            } else if (strcmp(buffer, "mem") == 0) {
                printf("[Daemon] CMD: mem\n");
                handle_mem_cmd(client_fd);
            } else if (strlen(buffer) > 0) {
                char *err = "ERROR: unknown command\n";
                send(client_fd, err, strlen(err), 0);
            }
        }
        close(client_fd);
    }

    cleanup_socket();
    return 0;
}