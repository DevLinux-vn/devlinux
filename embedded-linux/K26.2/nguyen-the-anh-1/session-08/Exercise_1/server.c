#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int server_fd;

void handle_sigint(int sig)
{
    (void)sig;

    unlink(SOCKET_PATH);
    close(server_fd);

    printf("\n[Daemon] Shutdown.\n");
    exit(0);
}

void handle_cpu(int client_fd)
{
    FILE *file;
    char line[BUFFER_SIZE];
    char load_avg[32];

    file = fopen("/proc/loadavg", "r");

    if (file == NULL) {
        const char *error = "ERROR: cannot read /proc/loadavg\n";
        send(client_fd, error, strlen(error), 0);
        return;
    }

    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);

        const char *error = "ERROR: cannot read load average\n";
        send(client_fd, error, strlen(error), 0);
        return;
    }

    fclose(file);

    sscanf(line, "%31s", load_avg);

    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "load_avg=%s\n", load_avg);

    if (send(client_fd, response, strlen(response), 0) == -1) {
        perror("send");
    }
}

void handle_mem(int client_fd)
{
    FILE *file;
    char line[BUFFER_SIZE];

    long mem_total = -1;
    long mem_free = -1;

    file = fopen("/proc/meminfo", "r");

    if (file == NULL) {
        const char *error = "ERROR: cannot read /proc/meminfo\n";
        send(client_fd, error, strlen(error), 0);
        return;
    }

    while (fgets(line, sizeof(line), file)) {

        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld kB", &mem_total);
        }

        if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %ld kB", &mem_free);
        }

        if (mem_total != -1 && mem_free != -1) {
            break;
        }
    }

    fclose(file);

    if (mem_total == -1 || mem_free == -1) {
        const char *error = "ERROR: cannot read memory information\n";
        send(client_fd, error, strlen(error), 0);
        return;
    }

    char response[BUFFER_SIZE];

    snprintf(response, sizeof(response),
             "mem_total=%ld kB mem_free=%ld kB\n",
             mem_total, mem_free);

    if (send(client_fd, response, strlen(response), 0) == -1) {
        perror("send");
    }
}

int main(void)
{
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    signal(SIGINT, handle_sigint);

    /* Remove old socket */
    unlink(SOCKET_PATH);

    /* Create socket */
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    /* Configure address */
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path,
            SOCKET_PATH,
            sizeof(addr.sun_path) - 1);

    /* Bind */
    if (bind(server_fd,
             (struct sockaddr *)&addr,
             sizeof(addr)) == -1) {

        perror("bind");
        close(server_fd);
        return 1;
    }

    /* Listen */
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    while (1) {

        /* Accept client */
        int client_fd = accept(server_fd, NULL, NULL);

        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        printf("[Daemon] Client connected.\n");

        while (1) {

            int bytes_received;

            bytes_received = recv(client_fd,
                                  buffer,
                                  sizeof(buffer) - 1,
                                  0);

            if (bytes_received == -1) {
                perror("recv");
                break;
            }

            if (bytes_received == 0) {
                printf("[Daemon] Client disconnected. "
                       "Waiting for next client...\n");
                break;
            }

            buffer[bytes_received] = '\0';

            /* Remove newline */
            buffer[strcspn(buffer, "\r\n")] = '\0';

            printf("[Daemon] CMD: %s\n", buffer);

            if (strcmp(buffer, "cpu") == 0) {

                handle_cpu(client_fd);

            } else if (strcmp(buffer, "mem") == 0) {

                handle_mem(client_fd);

            } else {

                const char *error =
                    "ERROR: unknown command\n";

                if (send(client_fd,
                         error,
                         strlen(error),
                         0) == -1) {
                    perror("send");
                    break;
                }
            }
        }

        close(client_fd);
    }

    return 0;
}
