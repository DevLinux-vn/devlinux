#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "iot_server.h"

static volatile sig_atomic_t g_shutdown = 0;
static client_t g_clients[MAX_CLIENTS];
static int g_listen_fd = -1;


static void handle_sigint(int signo)
{
    (void)signo;
    g_shutdown = 1;
}

static int send_all(int fd, const char *buf, size_t len)
{
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            perror("send");
            return -1;
        }
        sent += (size_t)n;
    }
    return 0;
}

static void client_slot_init(client_t *c)
{
    c->fd = -1;
    c->mode = 0;
    c->last_activity = 0;
    c->inlen = 0;
    c->inbuf[0] = '\0';
}

static int find_free_slot(void)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (g_clients[i].fd == -1)
            return i;
    return -1;
}

static int count_clients(void)
{
    int n = 0;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (g_clients[i].fd != -1)
            n++;
    return n;
}

static void close_client(int idx)
{
    if (g_clients[idx].fd != -1) {
        close(g_clients[idx].fd);
        printf("[Server] Client (fd=%d) disconnected\n", g_clients[idx].fd);
    }
    client_slot_init(&g_clients[idx]);
}

static void trim_newline(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

static int process_command(client_t *c, char *line)
{
    char response[BUF_SIZE];
    trim_newline(line);

    if (strcmp(line, "GET_TEMP") == 0) {
        double temp = 25.0 + (rand() % 10);
        int len = snprintf(response, sizeof(response), "%.1f\n", temp);
        printf("[Server] Client fd=%d: GET_TEMP -> %.1f\n", c->fd, temp);
        send_all(c->fd, response, (size_t)len);

    } else if (strcmp(line, "GET_HUMIDITY") == 0) {
        double hum = 30.0 + (rand() % 30);
        int len = snprintf(response, sizeof(response), "%.1f\n", hum);
        printf("[Server] Client fd=%d: GET_HUMIDITY -> %.1f\n", c->fd, hum);
        send_all(c->fd, response, (size_t)len);

    } else if (strncmp(line, "SET_MODE", 8) == 0) {
        int mode = -1;
        if (sscanf(line + 8, "%d", &mode) == 1 && mode >= 0 && mode <= 2) {
            c->mode = mode;
            printf("[Server] Client fd=%d: SET_MODE %d -> OK\n", c->fd, mode);
            send_all(c->fd, "OK\n", 3);
        } else {
            printf("[Server] Client fd=%d: SET_MODE malformed -> ERROR\n", c->fd);
            send_all(c->fd, "ERROR: Unknown command\n", 24);
        }

    } else if (strcmp(line, "QUIT") == 0) {
        printf("[Server] Client fd=%d: QUIT\n", c->fd);
        return -1;

    } else {
        printf("[Server] Client fd=%d: unknown command '%s'\n", c->fd, line);
        send_all(c->fd, "ERROR: Unknown command\n", 24);
    }

    c->last_activity = time(NULL);
    return 0;
}

static int handle_client_data(client_t *c)
{
    char tmp[BUF_SIZE];
    ssize_t n = recv(c->fd, tmp, sizeof(tmp) - 1, 0);

    if (n < 0) {
        if (errno == EINTR)
            return 0;
        perror("recv");
        return -1;
    }
    if (n == 0) {
        return -1;
    }
    tmp[n] = '\0';

    if (c->inlen + (size_t)n >= sizeof(c->inbuf)) {
        c->inlen = 0;
        c->inbuf[0] = '\0';
        send_all(c->fd, "ERROR: Unknown command\n", 24);
        return 0;
    }
    memcpy(c->inbuf + c->inlen, tmp, (size_t)n + 1);
    c->inlen += (size_t)n;
    char *start = c->inbuf;
    char *nl;
    while ((nl = strchr(start, '\n')) != NULL) {
        *nl = '\0';
        if (process_command(c, start) == -1)
            return -1; 
        start = nl + 1;
    }

    size_t remaining = strlen(start);
    memmove(c->inbuf, start, remaining + 1);
    c->inlen = remaining;

    return 0;
}

static void broadcast_status(void)
{
    double temp = 25.0 + (rand() % 10) + (rand() % 10) / 10.0;
    double hum  = 30.0 + (rand() % 30);
    int mode = 0;
    int nclients = count_clients();
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (g_clients[i].fd != -1) {
            mode = g_clients[i].mode;
        }
    }

    char msg[BUF_SIZE];
    int len = snprintf(msg, sizeof(msg),
                        "[SERVER_STATUS] Temp=%.1f Humidity=%.1f Mode=%d Clients=%d\n",
                        temp, hum, mode, nclients);

    printf("[Server] Broadcasting to %d clients: Temp=%.1f Humidity=%.1f Mode=%d Clients=%d\n",
           nclients, temp, hum, mode, nclients);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (g_clients[i].fd != -1) {
            send_all(g_clients[i].fd, msg, (size_t)len);
        }
    }
}

static void cleanup_and_exit(void)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (g_clients[i].fd != -1)
            close(g_clients[i].fd);
    }
    if (g_listen_fd != -1)
        close(g_listen_fd);
    printf("[Server] Shutdown complete.\n");
}

int main(void)
{
    struct sockaddr_in addr;
    int opt = 1;

    srand((unsigned int)time(NULL));
    for (int i = 0; i < MAX_CLIENTS; i++)
        client_slot_init(&g_clients[i]);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    g_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listen_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(g_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
    addr.sin_port = htons(SERVER_PORT);

    if (bind(g_listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(g_listen_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(g_listen_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        close(g_listen_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Server] Listening on %s:%d\n", SERVER_HOST, SERVER_PORT);

    time_t last_broadcast = time(NULL);

    while (!g_shutdown) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(g_listen_fd, &readfds);
        int max_fd = g_listen_fd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (g_clients[i].fd != -1) {
                FD_SET(g_clients[i].fd, &readfds);
                if (g_clients[i].fd > max_fd)
                    max_fd = g_clients[i].fd;
            }
        }

        struct timeval timeout;
        timeout.tv_sec = 1;   
        timeout.tv_usec = 0;

        int ready = select(max_fd + 1, &readfds, NULL, NULL, &timeout);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select");
            break;
        }

        if (ready > 0 && FD_ISSET(g_listen_fd, &readfds)) {
            struct sockaddr_in cli_addr;
            socklen_t cli_len = sizeof(cli_addr);
            int cfd = accept(g_listen_fd, (struct sockaddr *)&cli_addr, &cli_len);
            if (cfd < 0) {
                perror("accept");
            } else {
                int slot = find_free_slot();
                if (slot == -1) {
                    fprintf(stderr, "[Server] Max clients reached, rejecting connection\n");
                    close(cfd);
                } else {
                    g_clients[slot].fd = cfd;
                    g_clients[slot].mode = 0;
                    g_clients[slot].last_activity = time(NULL);
                    g_clients[slot].inlen = 0;
                    printf("[Server] Client connected from %s:%d (fd=%d)\n",
                           inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), cfd);
                }
            }
        }

        if (ready > 0) {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (g_clients[i].fd != -1 && FD_ISSET(g_clients[i].fd, &readfds)) {
                    if (handle_client_data(&g_clients[i]) == -1) {
                        close_client(i);
                    }
                }
            }
        }
        
        time_t now = time(NULL);
        if (difftime(now, last_broadcast) >= BROADCAST_INTERVAL) {
            broadcast_status();
            last_broadcast = now;
        }
    }

    cleanup_and_exit();
    return 0;
}