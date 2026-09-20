#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>

/* --- Constants (No Magic Numbers) --- */
#define SOCKET_PATH         "/tmp/monitor.sock"
#define MAX_BUFFER_SIZE     256
#define MAX_LINE_SIZE       128
#define MAX_BACKLOG         5

#define FILE_LOADAVG        "/proc/loadavg"
#define FILE_MEMINFO        "/proc/meminfo"

#define CMD_CPU             "cpu"
#define CMD_MEM             "mem"

#define MEM_TOTAL_KEY       "MemTotal:"
#define MEM_FREE_KEY        "MemFree:"

/* Global flag for graceful shutdown */
volatile sig_atomic_t g_is_running = 1;

/* --- Signal Handler --- */
static void handle_sigint(int signum) {
    (void)signum;
    g_is_running = 0;
}

/* --- Helper: Fetch CPU Load Average --- */
static void get_cpu_info(char *response, size_t max_len) {
    FILE *file = fopen(FILE_LOADAVG, "r");
    if (!file) {
        snprintf(response, max_len, "ERROR: cannot read loadavg");
        return;
    }

    double load1;
    if (fscanf(file, "%lf", &load1) == 1) {
        snprintf(response, max_len, "load_avg=%.2f", load1);
    } else {
        snprintf(response, max_len, "ERROR: invalid loadavg format");
    }
    
    fclose(file);
}

/* --- Helper: Fetch Memory Info --- */
static void get_mem_info(char *response, size_t max_len) {
    FILE *file = fopen(FILE_MEMINFO, "r");
    if (!file) {
        snprintf(response, max_len, "ERROR: cannot read meminfo");
        return;
    }

    long mem_total = -1;
    long mem_free = -1;
    char line[MAX_LINE_SIZE];

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, MEM_TOTAL_KEY, strlen(MEM_TOTAL_KEY)) == 0) {
            sscanf(line + strlen(MEM_TOTAL_KEY), "%ld", &mem_total);
        } else if (strncmp(line, MEM_FREE_KEY, strlen(MEM_FREE_KEY)) == 0) {
            sscanf(line + strlen(MEM_FREE_KEY), "%ld", &mem_free);
        }
    }
    fclose(file);

    if (mem_total != -1 && mem_free != -1) {
        snprintf(response, max_len, "mem_total=%ld kB mem_free=%ld kB", mem_total, mem_free);
    } else {
        snprintf(response, max_len, "ERROR: invalid meminfo format");
    }
}

/* --- Client Processing Loop --- */
static void process_client(int client_fd) {
    char buffer[MAX_BUFFER_SIZE];
    char response[MAX_BUFFER_SIZE];

    while (g_is_running) {
        ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_read < 0) {
            if (errno == EINTR) continue;
            perror("[Daemon] recv error");
            break;
        } else if (bytes_read == 0) {
            /* Client disconnected gracefully */
            break; 
        }

        /* Null-terminate the received data */
        buffer[bytes_read] = '\0';
        
        /* Strip trailing newlines if any */
        buffer[strcspn(buffer, "\r\n")] = '\0';

        printf("[Daemon] CMD: %s\n", buffer);

        if (strcmp(buffer, CMD_CPU) == 0) {
            get_cpu_info(response, sizeof(response));
        } else if (strcmp(buffer, CMD_MEM) == 0) {
            get_mem_info(response, sizeof(response));
        } else {
            snprintf(response, sizeof(response), "ERROR: unknown command");
        }

        /* Send response back to client */
        ssize_t bytes_sent = send(client_fd, response, strlen(response), 0);
        if (bytes_sent < 0) {
            perror("[Daemon] send error");
            break;
        }
    }
}

/* --- Main Entry --- */
int main(void) {
    /* Register SIGINT handler */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    /* Clean up stale socket file */
    unlink(SOCKET_PATH);

    /* 1. Create Socket */
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("[Daemon] socket failed");
        exit(EXIT_FAILURE);
    }

    /* 2. Bind Socket */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("[Daemon] bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* 3. Listen */
    if (listen(server_fd, MAX_BACKLOG) == -1) {
        perror("[Daemon] listen failed");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    /* 4. Accept & Serve Clients iteratively */
    while (g_is_running) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            if (errno == EINTR) {
                /* Interrupted by signal, gracefully exit */
                break;
            }
            perror("[Daemon] accept failed");
            continue;
        }

        printf("[Daemon] Client connected.\n");
        
        process_client(client_fd);
        
        printf("[Daemon] Client disconnected. Waiting for next client...\n");
        close(client_fd);
    }

    /* Cleanup on exit */
    printf("\n[Daemon] Shutting down...\n");
    close(server_fd);
    unlink(SOCKET_PATH);

    return EXIT_SUCCESS;
}
