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

static void handle_sigint(int sig)
{
    (void)sig;

    printf("\n[Daemon] Shutting down...\n");

    if (server_fd >= 0) {
        close(server_fd);
    }

    unlink(SOCKET_PATH);
    exit(EXIT_SUCCESS);
}

static int read_cpu(char *response, size_t response_size)
{
    FILE *file;
    char line[128];
    double load1;

    file = fopen("/proc/loadavg", "r");
    if (file == NULL) {
        return -1;
    }

    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return -1;
    }

    fclose(file);

    if (sscanf(line, "%lf", &load1) != 1) {
        return -1;
    }

    snprintf(response, response_size, "load_avg=%.2f\n", load1);

    return 0;
}

static int read_memory(char *response, size_t response_size)
{
    FILE *file;
    char line[128];

    unsigned long mem_total = 0;
    unsigned long mem_free = 0;

    file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "MemTotal: %lu kB", &mem_total) == 1) {
            continue;
        }

        if (sscanf(line, "MemFree: %lu kB", &mem_free) == 1) {
            continue;
        }

        if (mem_total != 0 && mem_free != 0) {
            break;
        }
    }

    fclose(file);

    if (mem_total == 0 || mem_free == 0) {
        return -1;
    }

    snprintf(response,
             response_size,
             "mem_total=%lu kB mem_free=%lu kB\n",
             mem_total,
             mem_free);

    return 0;
}

static void process_command(int client_fd, const char *command)
{
    char response[BUFFER_SIZE];

    if (strcmp(command, "cpu") == 0) {
        if (read_cpu(response, sizeof(response)) < 0) {
            snprintf(response,
                     sizeof(response),
                     "ERROR: cannot read CPU information\n");
        }
    }
    else if (strcmp(command, "mem") == 0) {
        if (read_memory(response, sizeof(response)) < 0) {
            snprintf(response,
                     sizeof(response),
                     "ERROR: cannot read memory information\n");
        }
    }
    else {
        snprintf(response,
                 sizeof(response),
                 "ERROR: unknown command\n");
    }

    if (send(client_fd,
             response,
             strlen(response),
             0) < 0) {
        perror("[Daemon] send");
    }
}

int main(void)
{
    struct sockaddr_un addr;

    signal(SIGINT, handle_sigint);

    /*
     * Remove stale socket file.
     */
    if (unlink(SOCKET_PATH) < 0 && errno != ENOENT) {
        perror("[Daemon] unlink");
        return EXIT_FAILURE;
    }

    /*
     * Create Unix Domain Stream Socket.
     */
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("[Daemon] socket");
        return EXIT_FAILURE;
    }

    /*
     * Configure socket address.
     */
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path,
            SOCKET_PATH,
            sizeof(addr.sun_path) - 1);

    /*
     * Bind socket.
     */
    if (bind(server_fd,
             (struct sockaddr *)&addr,
             sizeof(addr)) < 0) {
        perror("[Daemon] bind");
        close(server_fd);
        unlink(SOCKET_PATH);
        return EXIT_FAILURE;
    }

    /*
     * Listen for clients.
     */
    if (listen(server_fd, 5) < 0) {
        perror("[Daemon] listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return EXIT_FAILURE;
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    while (1) {
        int client_fd;

        /*
         * Wait for a client.
         */
        client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }

            perror("[Daemon] accept");
            break;
        }

        printf("[Daemon] Client connected.\n");

        while (1) {
            char buffer[BUFFER_SIZE];
            ssize_t bytes_received;

            memset(buffer, 0, sizeof(buffer));

            bytes_received = recv(client_fd,
                                  buffer,
                                  sizeof(buffer) - 1,
                                  0);

            if (bytes_received < 0) {
                if (errno == EINTR) {
                    continue;
                }

                perror("[Daemon] recv");
                break;
            }

            /*
             * Client closed connection.
             */
            if (bytes_received == 0) {
                printf("[Daemon] Client disconnected. "
                       "Waiting for next client...\n");
                break;
            }

            buffer[bytes_received] = '\0';

            /*
             * Remove newline from fgets() on client side.
             */
            buffer[strcspn(buffer, "\r\n")] = '\0';

            printf("[Daemon] CMD: %s\n", buffer);

            process_command(client_fd, buffer);
        }

        if (close(client_fd) < 0) {
            perror("[Daemon] close client");
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);

    return EXIT_SUCCESS;
}
