#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256
#define BACKLOG_QUEUE 5

volatile sig_atomic_t g_keep_running = 1;

static void handle_sigint(int signum) {
    (void)signum;
    g_keep_running = 0;
}

static void parse_cpu_load(char *response_out, size_t max_len) {
    double load1 = 0.0;
    FILE *f = fopen("/proc/loadavg", "r");
    if (f == NULL) {
        snprintf(response_out, max_len, "ERROR: Cannot read CPU stats");
        return;
    }
    if (fscanf(f, "%lf", &load1) != 1) {
        snprintf(response_out, max_len, "ERROR: Parse CPU stats failed");
    } else {
        snprintf(response_out, max_len, "load_avg=%.2f", load1);
    }
    fclose(f);
}

static void parse_mem_info(char *response_out, size_t max_len) {
    long mem_total = 0, mem_free = 0;
    char line[128];
    FILE *f = fopen("/proc/meminfo", "r");
    if (f == NULL) {
        snprintf(response_out, max_len, "ERROR: Cannot read Memory stats");
        return;
    }
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld", &mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %ld", &mem_free);
        }
    }
    fclose(f);
    snprintf(response_out, max_len, "mem_total=%ld kB mem_free=%ld kB", mem_total, mem_free);
}

int main(void) {
    setbuf(stdout, NULL);

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("CRITICAL: Failed to register SIGINT");
        return EXIT_FAILURE;
    }

    /* Xóa file socket cũ nếu còn tồn tại để tránh lỗi Address already in use */
    unlink(SOCKET_PATH);

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("CRITICAL: Socket creation failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0'; /* Bảo đảm an toàn chuỗi */

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("CRITICAL: Bind failed");
        close(server_fd);
        return EXIT_FAILURE;
    }

    if (listen(server_fd, BACKLOG_QUEUE) < 0) {
        perror("CRITICAL: Listen failed");
        close(server_fd);
        unlink(SOCKET_PATH);
        return EXIT_FAILURE;
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    while (g_keep_running) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            /* Nếu hàm accept bị gián đoạn do nhận tín hiệu SIGINT thì thoát vòng lặp an toàn */
            if (!g_keep_running) break;
            perror("ERROR: Accept failed");
            continue;
        }

        printf("[Daemon] Client connected.\n");

        char buffer[BUFFER_SIZE];
        ssize_t bytes_received;

        while (g_keep_running) {
            memset(buffer, 0, sizeof(buffer));
            bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

            if (bytes_received < 0) {
                perror("ERROR: Recv failed");
                break;
            } else if (bytes_received == 0) {
                printf("[Daemon] Client disconnected. Waiting for next client...\n");
                break;
            }

            /* Xử lý loại bỏ ký tự xuống dòng dính trong chuỗi lệnh */
            buffer[strcspn(buffer, "\r\n")] = '\0';
            printf("[Daemon] CMD: %s\n", buffer);

            char response[BUFFER_SIZE] = {0};
            if (strcmp(buffer, "cpu") == 0) {
                parse_cpu_load(response, sizeof(response));
            } else if (strcmp(buffer, "mem") == 0) {
                parse_mem_info(response, sizeof(response));
            } else {
                snprintf(response, sizeof(response), "ERROR: unknown command");
            }

            if (send(client_fd, response, strlen(response), 0) < 0) {
                perror("ERROR: Send failed");
                break;
            }
        }
        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    printf("[Daemon] Server stopped and cleaned up successfully.\n");
    return EXIT_SUCCESS;
}