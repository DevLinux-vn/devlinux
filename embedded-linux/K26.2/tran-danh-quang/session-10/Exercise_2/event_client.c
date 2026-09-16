#define _POSIX_C_SOURCE 200809L

#include "event_monitor.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static volatile sig_atomic_t g_client_running = 1;

static void sigint_handler(int signum) {
    (void)signum;
    g_client_running = 0;
}

static int setup_signal(void) {
    struct sigaction sa;
    (void)memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    (void)sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction(SIGINT)");
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

/* Connect to Unix domain control socket */
static int connect_to_monitor(const char *sock_path) {
    if (sock_path == NULL) {
        return STATUS_ERROR;
    }

    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket(AF_UNIX)");
        return STATUS_ERROR;
    }

    struct sockaddr_un addr;
    (void)memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    (void)strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect(AF_UNIX)");
        (void)close(sock_fd);
        return STATUS_ERROR;
    }

    return sock_fd;
}

/* Send a single command to monitor and receive response */
static int send_command(const char *sock_path, const char *cmd, char *out_resp, size_t resp_size) {
    if (sock_path == NULL || cmd == NULL) {
        return STATUS_ERROR;
    }

    int sock_fd = connect_to_monitor(sock_path);
    if (sock_fd < 0) {
        return STATUS_ERROR;
    }

    size_t cmd_len = strlen(cmd);
    ssize_t sent = send(sock_fd, cmd, cmd_len, 0);
    if (sent < 0) {
        perror("send");
        (void)close(sock_fd);
        return STATUS_ERROR;
    }

    if (out_resp != NULL && resp_size > 0) {
        (void)memset(out_resp, 0, resp_size);
        ssize_t received = recv(sock_fd, out_resp, resp_size - 1, 0);
        if (received > 0) {
            out_resp[received] = '\0';
        }
    }

    if (close(sock_fd) == -1) {
        perror("close(sock_fd)");
    }
    return STATUS_SUCCESS;
}

int main(void) {
    if (setvbuf(stdout, NULL, _IOLBF, 0) != 0) {
        perror("setvbuf(stdout)");
        return EXIT_FAILURE;
    }

    if (setup_signal() != STATUS_SUCCESS) {
        return EXIT_FAILURE;
    }

    char input_buf[BUFFER_SIZE];
    char resp_buf[BUFFER_SIZE];

    while (g_client_running) {
        (void)memset(input_buf, 0, sizeof(input_buf));
        if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
            /* EOF reached */
            break;
        }

        /* Check for exit / quit commands */
        if (strncmp(input_buf, "EXIT", 4) == 0 || strncmp(input_buf, "exit", 4) == 0 ||
            strncmp(input_buf, "quit", 4) == 0) {
            (void)send_command(SOCKET_PATH, "EXIT\n", NULL, 0);
            break;
        }

        /* Skip empty lines */
        if (input_buf[0] == '\n' || input_buf[0] == '\r') {
            continue;
        }

        (void)memset(resp_buf, 0, sizeof(resp_buf));
        if (send_command(SOCKET_PATH, input_buf, resp_buf, sizeof(resp_buf)) == STATUS_SUCCESS) {
            if (resp_buf[0] != '\0') {
                (void)printf("%s", resp_buf);
            }
        }
    }

    return EXIT_SUCCESS;
}
