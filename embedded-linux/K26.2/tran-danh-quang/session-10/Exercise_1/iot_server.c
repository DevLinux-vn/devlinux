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

/* Global termination flag controlled only by signal handler (async-signal-safe) */
static volatile sig_atomic_t g_server_running = 1;

/* Signal handler for graceful termination */
static void sigint_handler(int signum) {
    (void)signum;
    g_server_running = 0;
}

/* Setup signal action for SIGINT and SIGTERM */
static int setup_signal_handler(void) {
    struct sigaction sa;
    (void)memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    (void)sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction(SIGINT)");
        return STATUS_ERROR;
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction(SIGTERM)");
        return STATUS_ERROR;
    }
    /* Ignore SIGPIPE so server does not terminate when client abruptly disconnects */
    struct sigaction sa_pipe;
    (void)memset(&sa_pipe, 0, sizeof(sa_pipe));
    sa_pipe.sa_handler = SIG_IGN;
    (void)sigemptyset(&sa_pipe.sa_mask);
    sa_pipe.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa_pipe, NULL) == -1) {
        perror("sigaction(SIGPIPE)");
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

/* Initialize clients array */
static int init_clients(client_t *clients, int count) {
    if (clients == NULL || count <= 0) {
        return STATUS_ERROR;
    }
    for (int i = 0; i < count; i++) {
        clients[i].fd = -1;
        clients[i].mode = MODE_IDLE;
        clients[i].last_activity = 0;
    }
    return STATUS_SUCCESS;
}

/* Count active connected clients */
static int get_active_client_count(const client_t *clients, int max_clients) {
    if (clients == NULL) {
        return 0;
    }
    int count = 0;
    for (int i = 0; i < max_clients; i++) {
        if (clients[i].fd >= 0) {
            count++;
        }
    }
    return count;
}

/* Close a single client socket and reset slot */
static int disconnect_client(client_t *client, int client_index) {
    if (client == NULL || client->fd < 0) {
        return STATUS_ERROR;
    }

    if (close(client->fd) == -1) {
        perror("close(client_fd)");
    }
    (void)printf("[Server] Client %d disconnected.\n", client_index + 1);

    client->fd = -1;
    client->mode = MODE_IDLE;
    client->last_activity = 0;
    return STATUS_SUCCESS;
}

/* Safely send a complete buffer to socket */
static int send_all(int fd, const char *buf, size_t len) {
    if (fd < 0 || buf == NULL) {
        return STATUS_ERROR;
    }
    size_t total_sent = 0;
    while (total_sent < len) {
        ssize_t bytes = send(fd, buf + total_sent, len - total_sent, MSG_NOSIGNAL);
        if (bytes < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("send");
            return STATUS_ERROR;
        }
        if (bytes == 0) {
            return STATUS_ERROR;
        }
        total_sent += (size_t)bytes;
    }
    return STATUS_SUCCESS;
}

/* Broadcast server status line to all active clients */
static int broadcast_status(client_t *clients, int max_clients) {
    if (clients == NULL) {
        return STATUS_ERROR;
    }

    int active_clients = get_active_client_count(clients, max_clients);
    if (active_clients == 0) {
        return STATUS_SUCCESS;
    }

    /* Simulate dynamic sensor values */
    double temp = 25.0 + (rand() % 10) + ((rand() % 10) / 10.0);
    double humidity = 30.0 + (rand() % 30) + ((rand() % 10) / 10.0);
    int current_mode = MODE_ACTIVE;

    char broadcast_msg[BUFFER_SIZE];
    int len = snprintf(broadcast_msg, sizeof(broadcast_msg),
                       "[SERVER_STATUS] Temp=%.1f Humidity=%.1f Mode=%d Clients=%d\n",
                       temp, humidity, current_mode, active_clients);
    if (len < 0 || (size_t)len >= sizeof(broadcast_msg)) {
        return STATUS_ERROR;
    }

    (void)printf("[Server] Broadcasting to %d clients: Temp=%.1f Humidity=%.1f Mode=%d Clients=%d\n",
                 active_clients, temp, humidity, current_mode, active_clients);

    for (int i = 0; i < max_clients; i++) {
        if (clients[i].fd >= 0) {
            if (send_all(clients[i].fd, broadcast_msg, (size_t)len) != STATUS_SUCCESS) {
                (void)disconnect_client(&clients[i], i);
            }
        }
    }
    return STATUS_SUCCESS;
}

/* Parse and execute a single client command */
static int handle_client_command(client_t *client, int client_index, const char *cmd) {
    if (client == NULL || cmd == NULL) {
        return STATUS_ERROR;
    }

    char response[BUFFER_SIZE];
    (void)memset(response, 0, sizeof(response));

    if (strncmp(cmd, "GET_TEMP", 8) == 0 && (cmd[8] == '\0' || cmd[8] == '\r' || cmd[8] == '\n' || cmd[8] == ' ')) {
        double temp = 25.0 + (rand() % 10) + ((rand() % 10) / 10.0);
        int len = snprintf(response, sizeof(response), "%.1f\n", temp);
        if (len > 0) {
            (void)printf("[Server] Client %d: GET_TEMP -> %.1f\n", client_index + 1, temp);
            return send_all(client->fd, response, (size_t)len);
        }
        return STATUS_ERROR;
    }

    if (strncmp(cmd, "GET_HUMIDITY", 12) == 0 && (cmd[12] == '\0' || cmd[12] == '\r' || cmd[12] == '\n' || cmd[12] == ' ')) {
        double humidity = 30.0 + (rand() % 30) + ((rand() % 10) / 10.0);
        int len = snprintf(response, sizeof(response), "%.1f\n", humidity);
        if (len > 0) {
            (void)printf("[Server] Client %d: GET_HUMIDITY -> %.1f\n", client_index + 1, humidity);
            return send_all(client->fd, response, (size_t)len);
        }
        return STATUS_ERROR;
    }

    if (strncmp(cmd, "SET_MODE", 8) == 0) {
        int parsed_mode = -1;
        if (sscanf(cmd + 8, "%d", &parsed_mode) == 1 && parsed_mode >= MODE_IDLE && parsed_mode <= MODE_ALERT) {
            client->mode = parsed_mode;
            int len = snprintf(response, sizeof(response), "OK\n");
            if (len > 0) {
                (void)printf("[Server] Client %d: SET_MODE %d -> OK\n", client_index + 1, parsed_mode);
                return send_all(client->fd, response, (size_t)len);
            }
            return STATUS_ERROR;
        }
        int len = snprintf(response, sizeof(response), "ERROR: Invalid mode (0=idle, 1=active, 2=alert)\n");
        if (len > 0) {
            return send_all(client->fd, response, (size_t)len);
        }
        return STATUS_ERROR;
    }

    if (strncmp(cmd, "QUIT", 4) == 0 && (cmd[4] == '\0' || cmd[4] == '\r' || cmd[4] == '\n' || cmd[4] == ' ')) {
        (void)printf("[Server] Client %d requested QUIT.\n", client_index + 1);
        return disconnect_client(client, client_index);
    }

    /* Unknown command response */
    int len = snprintf(response, sizeof(response), "ERROR: Unknown command\n");
    if (len > 0) {
        (void)printf("[Server] Client %d: Unknown command [%s]\n", client_index + 1, cmd);
        return send_all(client->fd, response, (size_t)len);
    }
    return STATUS_ERROR;
}

/* Read incoming data from a client socket and handle lines */
static int process_client_data(client_t *client, int client_index) {
    if (client == NULL || client->fd < 0) {
        return STATUS_ERROR;
    }

    char recv_buf[BUFFER_SIZE];
    (void)memset(recv_buf, 0, sizeof(recv_buf));

    ssize_t bytes_read = recv(client->fd, recv_buf, sizeof(recv_buf) - 1, 0);
    if (bytes_read <= 0) {
        if (bytes_read < 0 && errno == EINTR) {
            return STATUS_SUCCESS;
        }
        /* Client disconnected or read error */
        return disconnect_client(client, client_index);
    }

    recv_buf[bytes_read] = '\0';
    time_t now = time(NULL);
    if (now != (time_t)-1) {
        client->last_activity = now;
    }

    /* Process line by line if newline present */
    char *line = strtok(recv_buf, "\r\n");
    while (line != NULL) {
        /* Skip leading whitespace */
        while (*line == ' ') {
            line++;
        }
        if (*line != '\0') {
            if (handle_client_command(client, client_index, line) != STATUS_SUCCESS) {
                break;
            }
        }
        line = strtok(NULL, "\r\n");
    }
    return STATUS_SUCCESS;
}

/* Accept a new incoming client connection */
static int accept_new_client(int listen_fd, client_t *clients, int max_clients) {
    if (listen_fd < 0 || clients == NULL) {
        return STATUS_ERROR;
    }

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    (void)memset(&client_addr, 0, sizeof(client_addr));

    int new_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (new_fd < 0) {
        if (errno == EINTR) {
            return STATUS_SUCCESS;
        }
        perror("accept");
        return STATUS_ERROR;
    }

    char client_ip[INET_ADDRSTRLEN];
    (void)memset(client_ip, 0, sizeof(client_ip));
    if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip)) == NULL) {
        perror("inet_ntop");
        (void)strncpy(client_ip, "unknown", sizeof(client_ip) - 1);
    }
    int client_port = ntohs(client_addr.sin_port);

    /* Find empty slot */
    for (int i = 0; i < max_clients; i++) {
        if (clients[i].fd < 0) {
            clients[i].fd = new_fd;
            clients[i].mode = MODE_IDLE;
            clients[i].last_activity = time(NULL);

            (void)printf("[Server] Client %d connected from %s:%d\n", i + 1, client_ip, client_port);
            return STATUS_SUCCESS;
        }
    }

    /* If maximum capacity reached, notify and close */
    const char *reject_msg = "ERROR: Server is full\n";
    (void)send(new_fd, reject_msg, strlen(reject_msg), 0);
    if (close(new_fd) == -1) {
        perror("close(rejected_fd)");
    }
    (void)fprintf(stderr, "[Server] Rejected connection from %s:%d (Max clients reached)\n",
                  client_ip, client_port);
    return STATUS_SUCCESS;
}

/* Close all active connections and release resources */
static int cleanup_server(int listen_fd, client_t *clients, int max_clients) {
    if (clients != NULL) {
        for (int i = 0; i < max_clients; i++) {
            if (clients[i].fd >= 0) {
                if (close(clients[i].fd) == -1) {
                    perror("close(client_fd)");
                }
                clients[i].fd = -1;
            }
        }
    }
    if (listen_fd >= 0) {
        if (close(listen_fd) == -1) {
            perror("close(listen_fd)");
        }
    }
    (void)printf("[Server] Shutdown complete.\n");
    return STATUS_SUCCESS;
}

int main(void) {
    /* Setup line buffering for predictable stdout */
    if (setvbuf(stdout, NULL, _IOLBF, 0) != 0) {
        perror("setvbuf(stdout)");
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));

    if (setup_signal_handler() != STATUS_SUCCESS) {
        return EXIT_FAILURE;
    }

    client_t clients[MAX_CLIENTS];
    if (init_clients(clients, MAX_CLIENTS) != STATUS_SUCCESS) {
        return EXIT_FAILURE;
    }

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR)");
        (void)close(listen_fd);
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP_ADDR, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        (void)close(listen_fd);
        return EXIT_FAILURE;
    }

    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        (void)close(listen_fd);
        return EXIT_FAILURE;
    }

    if (listen(listen_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        (void)close(listen_fd);
        return EXIT_FAILURE;
    }

    (void)printf("[Server] Listening on %s:%d\n", SERVER_IP_ADDR, SERVER_PORT);

    time_t last_broadcast = time(NULL);

    /* Main event loop driven by select() */
    while (g_server_running) {
        fd_set read_fds;
        FD_ZERO(&read_fds);

        FD_SET(listen_fd, &read_fds);
        int max_fd = listen_fd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd >= 0) {
                FD_SET(clients[i].fd, &read_fds);
                if (clients[i].fd > max_fd) {
                    max_fd = clients[i].fd;
                }
            }
        }

        /* 1 second timeout allows timely periodic broadcast checks */
        struct timeval timeout;
        timeout.tv_sec = SELECT_TIMEOUT_SEC;
        timeout.tv_usec = 0;

        int ready = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select");
            break;
        }

        /* Check for new connection requests */
        if (FD_ISSET(listen_fd, &read_fds)) {
            (void)accept_new_client(listen_fd, clients, MAX_CLIENTS);
        }

        /* Check for incoming data from clients */
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd >= 0 && FD_ISSET(clients[i].fd, &read_fds)) {
                (void)process_client_data(&clients[i], i);
            }
        }

        /* Check periodic 5-second broadcast interval */
        time_t current_time = time(NULL);
        if (current_time != (time_t)-1 && (current_time - last_broadcast) >= BROADCAST_INTERVAL) {
            (void)broadcast_status(clients, MAX_CLIENTS);
            last_broadcast = current_time;
        }
    }

    (void)cleanup_server(listen_fd, clients, MAX_CLIENTS);
    return EXIT_SUCCESS;
}
