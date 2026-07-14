#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_PORT 9000
#define BUFFER_SIZE 128

int main() {
    // 1. socket(AF_INET, SOCK_DGRAM, 0)
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. setsockopt(SO_REUSEADDR) to avoid "Address already in use"
    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt SO_REUSEADDR failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Configure server address (0.0.0.0:9000)
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(COLLECTOR_PORT);

    // 3. bind()
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Collector] Listening on 0.0.0.0:%d...\n", COLLECTOR_PORT);

    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 4. Loop continuous until Ctrl+C
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        
        // Receive datagram
        ssize_t n = recvfrom(sock_fd, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr *)&client_addr, &client_len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        // Null-terminate string
        buffer[n] = '\0';

        // Get current timestamp [HH:MM:SS]
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char ts[16];
        strftime(ts, sizeof(ts), "%H:%M:%S", t);

        // Convert client IP and Port to readable format
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);

        // Print exactly as expected output
        printf("[%s] %s:%d -> %s\n", ts, client_ip, client_port, buffer);
    }

    close(sock_fd);
    return 0;
}