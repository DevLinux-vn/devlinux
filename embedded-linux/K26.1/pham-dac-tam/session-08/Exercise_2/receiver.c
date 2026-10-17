#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

#define BUFFER_SIZE    128
#define COLLECTOR_PORT 9000

int collector_fd = -1;

void cleanup(int sig) {
    if (collector_fd != -1) {
        close(collector_fd);
    }
    printf("\n[Collector] Shutdown.\n");
    exit(0);
}

int main() {
    signal(SIGINT, cleanup);
    
    // Create UDP socket
    collector_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (collector_fd == -1) {
        perror("socket");
        exit(1);
    }
    
    // Enable SO_REUSEADDR
    int reuse = 1;
    if (setsockopt(collector_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt");
        close(collector_fd);
        exit(1);
    }
    
    // Bind to 0.0.0.0:9000
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(COLLECTOR_PORT);
    
    if (bind(collector_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(collector_fd);
        exit(1);
    }
    
    printf("[Collector] Listening on 0.0.0.0:%d...\n", COLLECTOR_PORT);
    
    char buffer[BUFFER_SIZE];
    struct sockaddr_in sender_addr;
    socklen_t sender_addr_len = sizeof(sender_addr);
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        
        ssize_t n = recvfrom(collector_fd, buffer, BUFFER_SIZE - 1, 0,
                             (struct sockaddr *)&sender_addr, &sender_addr_len);
        
        if (n == -1) {
            perror("recvfrom");
            continue;
        }
        
        buffer[n] = '\0';
        
        // Get current timestamp
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char ts[16];
        strftime(ts, sizeof(ts), "%H:%M:%S", t);
        
        // Get sender IP and port
        char sender_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sender_addr.sin_addr, sender_ip, INET_ADDRSTRLEN);
        int sender_port = ntohs(sender_addr.sin_port);
        
        printf("[%s] %s:%d → %s\n", ts, sender_ip, sender_port, buffer);
    }
    
    return 0;
}
