#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int server_fd = -1;

void cleanup(int sig) {
    if (server_fd != -1) {
        close(server_fd);
    }
    (void)sig; // suppress unused parameter warning
    unlink(SOCKET_PATH);
    printf("\n[Daemon] Cleanup done. Goodbye.\n");
    exit(0);
}

void read_cpu_load(char *response, size_t size) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) {
        snprintf(response, size, "ERROR: cannot read /proc/loadavg");
        return;
    }
    
    double load1;
    fscanf(f, "%lf", &load1);
    fclose(f);
    
    snprintf(response, size, "load_avg=%.2f", load1);
}

void read_mem_info(char *response, size_t size) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        snprintf(response, size, "ERROR: cannot read /proc/meminfo");
        return;
    }
    
    long mem_total = 0, mem_free = 0;
    char line[128];
    
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld", &mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %ld", &mem_free);
        }
    }
    fclose(f);
    
    snprintf(response, size, "mem_total=%ld kB mem_free=%ld kB", mem_total, mem_free);
}

int main() {
    signal(SIGINT, cleanup); // nhan Ctrl C goi cleanup don dep

    unlink(SOCKET_PATH); // xoa file socket cu neu co

    // Create Unix Domain Socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) { // tao socket that bai
        perror("socket");
        exit(1);
    }
    // Bind to socket path
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    // Bind socket to the address
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    //  Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        exit(1);
    }

    printf("[Daemon] Server is running. Waiting for connections on %s...\n", SOCKET_PATH);

    while (1) {
        // Accept client
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }
        
        printf("[Daemon] Client connected.\n");
        
        // Command loop
        char buffer[BUFFER_SIZE];
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t n = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            
            if (n == -1) {
                perror("recv");
                break;
            }
            
            if (n == 0) {
                // Client disconnected
                printf("[Daemon] Client disconnected. Waiting for next client...\n");
                break;
            }
            
            // Remove trailing newline/whitespace
            buffer[n] = '\0';
            for (int i = n - 1; i >= 0; i--) {
                if (buffer[i] == '\n' || buffer[i] == '\r') {
                    buffer[i] = '\0';
                } else {
                    break;
                }
            }
            
            printf("[Daemon] CMD: %s\n", buffer);
            
            char response[BUFFER_SIZE];
            
            if (strcmp(buffer, "cpu") == 0) {
                read_cpu_load(response, sizeof(response));
            } else if (strcmp(buffer, "mem") == 0) {
                read_mem_info(response, sizeof(response));
            } else {
                snprintf(response, sizeof(response), "ERROR: unknown command");
            }
            
            // Send response
            if (send(client_fd, response, strlen(response), 0) == -1) {
                perror("send");
                break;
            }
        }
        
        close(client_fd);
    }
}
