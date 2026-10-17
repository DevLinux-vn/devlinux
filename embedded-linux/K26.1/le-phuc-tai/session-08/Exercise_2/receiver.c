#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_PORT 9000
#define BUFFER_SIZE 256

volatile sig_atomic_t g_keep_running = 1;

static void handle_sigint(int signum) {
    (void)signum;
    g_keep_running = 0;
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

    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) {
        perror("CRITICAL: Socket creation failed");
        return EXIT_FAILURE;
    }

    /* SỬA LỖI DOANH NGHIỆP: Bật SO_REUSEADDR tái sử dụng cổng lập tức khi restart */
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("CRITICAL: setsockopt SO_REUSEADDR failed");
        close(server_fd);
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; /* Lắng nghe trên mọi interface (0.0.0.0) */
    addr.sin_port = htons(COLLECTOR_PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("CRITICAL: Bind failed");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("[Collector] Listening on 0.0.0.0:%d...\n", COLLECTOR_PORT);

    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (g_keep_running) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recvfrom(server_fd, buffer, sizeof(buffer) - 1, 0,
                                          (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received < 0) {
            if (!g_keep_running) break;
            perror("ERROR: recvfrom failed");
            continue;
        }

        /* Lấy mốc thời gian thực tại theo định dạng yêu cầu */
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char ts[16];
        strftime(ts, sizeof(ts), "%H:%M:%S", t);

        /* Trích xuất thông tin IP và Cổng của thiết bị gửi tới */
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);

        printf("[%s] %s:%d → %s\n", ts, client_ip, client_port, buffer);
    }

    close(server_fd);
    printf("[Collector] Server stopped cleanly.\n");
    return EXIT_SUCCESS;
}