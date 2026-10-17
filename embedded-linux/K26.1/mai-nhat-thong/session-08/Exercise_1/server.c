/**
 * @file server.c
 * @brief System Resource Monitor Daemon using Unix Domain Stream Socket.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

/* Atomic flag for graceful termination */
static volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

/**
 * @brief Retrieves CPU load average from /proc/loadavg
 */
void get_cpu_load(char *response, size_t size) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) {
        snprintf(response, size, "ERROR: Cannot read /proc/loadavg");
        return;
    }
    float load1;
    if (fscanf(f, "%f", &load1) == 1) {
        snprintf(response, size, "load_avg=%.2f", load1);
    } else {
        snprintf(response, size, "ERROR: Parsing /proc/loadavg failed");
    }
    fclose(f);
}

/**
 * @brief Retrieves Memory information from /proc/meminfo
 */
void get_mem_info(char *response, size_t size) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        snprintf(response, size, "ERROR: Cannot read /proc/meminfo");
        return;
    }
    
    char line[128];
    long mem_total = 0, mem_free = 0;
    
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld kB", &mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %ld kB", &mem_free);
        }
    }
    fclose(f);
    
    snprintf(response, size, "mem_total=%ld kB mem_free=%ld kB", mem_total, mem_free);
}

int main(void) {
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    /* Setup safe signal handling */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    /* 1. Remove stale socket if it exists */
    unlink(SOCKET_PATH);

    /* 2. Create Unix Domain Socket */
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    /* 3. Bind the socket */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* 4. Listen for incoming connections */
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    /* 5. Main Accept Loop */
    while (keep_running) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            if (!keep_running) break; /* Interrupted by SIGINT */
            perror("Accept failed");
            continue;
        }

        printf("[Daemon] Client connected.\n");

        /* Command Processing Loop for the connected client */
        while (keep_running) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            
            if (bytes_read <= 0) {
                printf("[Daemon] Client disconnected. Waiting for next client...\n");
                break; /* Client closed connection or error occurred */
            }

            /* Clean up trailing newlines from client input */
            buffer[strcspn(buffer, "\r\n")] = 0;
            printf("[Daemon] CMD: %s\n", buffer);

            char response[BUFFER_SIZE];
            if (strcmp(buffer, "cpu") == 0) {
                get_cpu_load(response, sizeof(response));
            } else if (strcmp(buffer, "mem") == 0) {
                get_mem_info(response, sizeof(response));
            } else {
                snprintf(response, sizeof(response), "ERROR: unknown command");
            }

            /* Send response back to client */
            /* Add newline to match client expectations if needed, but string itself is enough */
            strncat(response, "\n", sizeof(response) - strlen(response) - 1);
            if (send(client_fd, response, strlen(response), 0) == -1) {
                perror("Send failed");
                break;
            }
        }
        close(client_fd);
    }

    /* Cleanup on exit */
    printf("\n[Daemon] Shutting down. Cleaning up socket.\n");
    close(server_fd);
    unlink(SOCKET_PATH);
    return EXIT_SUCCESS;
}