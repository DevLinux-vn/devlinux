#define _POSIX_C_SOURCE 200809L

#include "iot_server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

/* Global running flag for client termination */
static volatile sig_atomic_t g_client_running = 1;

static void sigint_handler(int signum) {
    (void)signum;
    g_client_running = 0;
}

static int setup_client_signal(void) {
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

/* Print incoming server message, formatting server status as broadcast if matching */
static int handle_server_message(const char *msg) {
    if (msg == NULL) {
        return STATUS_ERROR;
    }
    if (strncmp(msg, "[SERVER_STATUS] ", 16) == 0) {
        (void)printf("[BROADCAST] %s", msg + 16);
    } else {
        (void)printf("%s", msg);
    }
    return STATUS_SUCCESS;
}

/* Process user input from standard input */
static int handle_stdin_input(int sock_fd) {
    char input_buf[BUFFER_SIZE];
    (void)memset(input_buf, 0, sizeof(input_buf));

    if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
        /* EOF reached (Ctrl+D) */
        g_client_running = 0;
        return STATUS_SUCCESS;
    }

    /* Check for exit / quit commands */
    if (strncmp(input_buf, "quit", 4) == 0 || strncmp(input_buf, "QUIT", 4) == 0) {
        /* Send QUIT to server before closing */
        (void)send(sock_fd, "QUIT\n", 5, MSG_NOSIGNAL);
        g_client_running = 0;
        return STATUS_SUCCESS;
    }

    size_t len = strlen(input_buf);
    if (len > 0) {
        ssize_t sent = send(sock_fd, input_buf, len, MSG_NOSIGNAL);
        if (sent < 0) {
            perror("send");
            return STATUS_ERROR;
        }
    }
    return STATUS_SUCCESS;
}

int main(void) {
    if (setvbuf(stdout, NULL, _IOLBF, 0) != 0) {
        perror("setvbuf(stdout)");
        return EXIT_FAILURE;
    }

    if (setup_client_signal() != STATUS_SUCCESS) {
        return EXIT_FAILURE;
    }

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP_ADDR, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        (void)close(sock_fd);
        return EXIT_FAILURE;
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        (void)close(sock_fd);
        return EXIT_FAILURE;
    }

    /* Loop using select to multiplex stdin and socket */
    while (g_client_running) {
        fd_set read_fds;
        FD_ZERO(&read_fds);

        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(sock_fd, &read_fds);

        int max_fd = (sock_fd > STDIN_FILENO) ? sock_fd : STDIN_FILENO;

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select");
            break;
        }

        /* Check data from server */
        if (FD_ISSET(sock_fd, &read_fds)) {
            char recv_buf[BUFFER_SIZE];
            (void)memset(recv_buf, 0, sizeof(recv_buf));
            ssize_t bytes_recv = recv(sock_fd, recv_buf, sizeof(recv_buf) - 1, 0);
            if (bytes_recv <= 0) {
                if (bytes_recv < 0 && errno == EINTR) {
                    continue;
                }
                (void)printf("\n[Client] Server disconnected.\n");
                break;
            }
            recv_buf[bytes_recv] = '\0';
            (void)handle_server_message(recv_buf);
        }

        /* Check user input from stdin */
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (handle_stdin_input(sock_fd) != STATUS_SUCCESS) {
                break;
            }
        }
    }

    if (close(sock_fd) == -1) {
        perror("close(sock_fd)");
    }
    return EXIT_SUCCESS;
}
