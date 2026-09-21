#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256
#define BACKLOG     5

static int listen_fd = -1; 
static void handle_sigint(int signo)
{
    (void)signo;

    const char msg[] = "\n[Daemon] Caught SIGINT, shutting down...\n";
    /* write() is async-signal-safe, printf() is not */
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);

    if (listen_fd != -1) {
        close(listen_fd);
    }
    unlink(SOCKET_PATH);
    _exit(0);
}

static void build_cpu_response(char *out, size_t out_size)
{
    FILE *f = fopen("/proc/loadavg", "r");
    double load1 = 0.0;

    if (f == NULL) {
        snprintf(out, out_size, "ERROR: cannot read /proc/loadavg");
        return;
    }

    if (fscanf(f, "%lf", &load1) != 1) {
        fclose(f);
        snprintf(out, out_size, "ERROR: cannot parse /proc/loadavg");
        return;
    }
    fclose(f);

    snprintf(out, out_size, "load_avg=%.2f", load1);
}

static void build_mem_response(char *out, size_t out_size)
{
    FILE *f = fopen("/proc/meminfo", "r");
    char line[128];
    long mem_total = -1, mem_free = -1;

    if (f == NULL) {
        snprintf(out, out_size, "ERROR: cannot read /proc/meminfo");
        return;
    }

    while (fgets(line, sizeof(line), f) != NULL) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 9, "%ld", &mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 8, "%ld", &mem_free);
        }
        if (mem_total != -1 && mem_free != -1) {
            break;
        }
    }
    fclose(f);

    if (mem_total == -1 || mem_free == -1) {
        snprintf(out, out_size, "ERROR: cannot parse /proc/meminfo");
        return;
    }

    snprintf(out, out_size, "mem_total=%ld kB mem_free=%ld kB",
             mem_total, mem_free);
}


static void trim_newline(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

int main(void)
{
    struct sockaddr_un addr;
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (unlink(SOCKET_PATH) == -1 && errno != ENOENT) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, BACKLOG) == -1) {
        perror("listen");
        close(listen_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);
    fflush(stdout);

    for (;;) {
        int client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd == -1) {
            if (errno == EINTR) {
                continue; /* interrupted by a signal, retry */
            }
            perror("accept");
            continue;
        }

        printf("[Daemon] Client connected.\n");
        fflush(stdout);

        char buffer[BUFFER_SIZE];
        for (;;) {
            memset(buffer, 0, sizeof(buffer));
            ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

            if (n == -1) {
                perror("recv");
                break;
            }
            if (n == 0) {
                printf("[Daemon] Client disconnected. Waiting for next client...\n");
                fflush(stdout);
                break;
            }

            buffer[n] = '\0';
            trim_newline(buffer);

            char response[BUFFER_SIZE];
            memset(response, 0, sizeof(response));

            if (strcmp(buffer, "cpu") == 0) {
                printf("[Daemon] CMD: cpu\n");
                build_cpu_response(response, sizeof(response));
            } else if (strcmp(buffer, "mem") == 0) {
                printf("[Daemon] CMD: mem\n");
                build_mem_response(response, sizeof(response));
            } else {
                printf("[Daemon] CMD: %s (unknown)\n", buffer);
                snprintf(response, sizeof(response), "ERROR: unknown command");
            }
            fflush(stdout);

            ssize_t sent = send(client_fd, response, strlen(response), 0);
            if (sent == -1) {
                perror("send");
                break;
            }
        }

        close(client_fd);
    }

    close(listen_fd);
    unlink(SOCKET_PATH);
    return 0;
}